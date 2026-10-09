#include "freewake/commands/AirfoilInfoCommand.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <set>
#include <iomanip>
#include <algorithm>

namespace FreeWake {

AirfoilInfoCommand::AirfoilInfoCommand(AirfoilInfoConfig config)
    : config_(std::move(config))
{
}

int AirfoilInfoCommand::inspectFile(const std::string& path)
{
    std::ifstream fin(path);
    if (!fin.good()) {
        std::cerr << "[FreeWake Error]: Cannot open airfoil file '" << path << "'\n";
        return 1;
    }

    std::string headerLine;
    if (!std::getline(fin, headerLine)) {
        std::cerr << "[FreeWake Error]: File '" << path << "' is empty.\n";
        return 1;
    }

    std::string name = headerLine;
    int expectedRows = -1;
    auto eqPos = headerLine.find('=');
    if (eqPos != std::string::npos) {
        name = headerLine.substr(0, eqPos);
        // Trim trailing title text like "Number of Rows"
        auto numPos = name.rfind("Number");
        if (numPos != std::string::npos) {
            name = name.substr(0, numPos);
        }
        std::string rowStr = headerLine.substr(eqPos + 1);
        std::stringstream ss(rowStr);
        ss >> expectedRows;
    }

    // Trim whitespace
    while (!name.empty() && std::isspace(name.back())) name.pop_back();

    int rowCount = 0;
    std::set<double> reynoldsNumbers;
    double minAlpha = 1e9, maxAlpha = -1e9;
    double minCl = 1e9, maxCl = -1e9;
    double minCd = 1e9, maxCd = -1e9;
    double minCm = 1e9, maxCm = -1e9;
    bool hasNegativeCd = false;

    double a, cl, cd, re, cm;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#' || line[0] == '%') continue;
        std::stringstream ss(line);
        if (ss >> a >> cl >> cd >> re >> cm) {
            rowCount++;
            reynoldsNumbers.insert(re);
            minAlpha = std::min(minAlpha, a); maxAlpha = std::max(maxAlpha, a);
            minCl = std::min(minCl, cl); maxCl = std::max(maxCl, cl);
            minCd = std::min(minCd, cd); maxCd = std::max(maxCd, cd);
            minCm = std::min(minCm, cm); maxCm = std::max(maxCm, cm);
            if (cd < 0.0) hasNegativeCd = true;
        }
    }

    bool valid = (expectedRows <= 0 || rowCount == expectedRows) && !hasNegativeCd && (rowCount > 0);

    std::cout << "---------------------------------------------------------------------------\n";
    std::cout << " Airfoil: " << std::filesystem::path(path).filename().string() << " (" << name << ")\n";
    std::cout << "---------------------------------------------------------------------------\n";
    std::cout << "  Status:          " << (valid ? "[VALID PASS]" : "[VALIDATION WARNING]") << "\n";
    std::cout << "  Rows read:       " << rowCount;
    if (expectedRows > 0) {
        std::cout << " (Header expected: " << expectedRows << ")";
    }
    std::cout << "\n";
    std::cout << "  Reynolds Count:  " << reynoldsNumbers.size() << " distinct Re values: {";
    size_t count = 0;
    for (double r : reynoldsNumbers) {
        if (count++ > 0) std::cout << ", ";
        std::cout << std::scientific << std::setprecision(2) << r;
    }
    std::cout << std::defaultfloat << "}\n";
    std::cout << "  Alpha range:     [" << minAlpha << " deg, " << maxAlpha << " deg]\n";
    std::cout << "  Cl range:        [" << minCl << ", " << maxCl << "]\n";
    std::cout << "  Cd range:        [" << minCd << ", " << maxCd << "]\n";
    std::cout << "  Cm range:        [" << minCm << ", " << maxCm << "]\n";
    if (hasNegativeCd) {
        std::cout << "  WARNING: Negative drag coefficient detected in polar data!\n";
    }
    std::cout << "\n";

    return valid ? 0 : 1;
}

int AirfoilInfoCommand::execute()
{
    std::cout << "===========================================================================\n";
    std::cout << " FreeWake Airfoil Data Inspection & Validation\n";
    std::cout << "===========================================================================\n";

    if (!config_.file_path.empty()) {
        return inspectFile(config_.file_path);
    }

    // Inspect all files in airfoils_dir
    std::string dir = config_.airfoils_dir;
    if (!std::filesystem::exists(dir)) {
        std::cerr << "[FreeWake Error]: Airfoil directory '" << dir << "' does not exist.\n";
        return 1;
    }

    int filesChecked = 0;
    int errors = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        std::string fname = entry.path().filename().string();
        if (fname.rfind(".", 0) == 0 || fname.rfind("__", 0) == 0) continue;
        if (entry.is_regular_file() && entry.path().extension() == ".dat") {
            filesChecked++;
            if (inspectFile(entry.path().string()) != 0) {
                errors++;
            }
        }
    }

    std::cout << "Summary: Inspected " << filesChecked << " airfoil files ("
              << (filesChecked - errors) << " passed, " << errors << " warnings/errors).\n";
    return (errors == 0) ? 0 : 1;
}

} // namespace FreeWake
