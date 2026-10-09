#include "freewake/commands/PointVelocityCommand.hpp"
#include "freewake/general.hpp"
#include "freewake/induced_velocity.hpp"
#include "freewake/read_input.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <cmath>

namespace FreeWake {

PointVelocityCommand::PointVelocityCommand(PointVelocityConfig config)
    : config_(std::move(config))
{
}

int PointVelocityCommand::execute()
{
    double P[3] = {0.0, 0.0, 0.0};
    int timestep = config_.timestep;

    // 1. Determine point coordinates
    if (!config_.point_str.empty()) {
        std::string s = config_.point_str;
        for (char& c : s) {
            if (c == ',') c = ' ';
        }
        std::stringstream ss(s);
        if (!(ss >> P[0] >> P[1] >> P[2])) {
            std::cerr << "[FreeWake Error]: Failed to parse point coordinates from '"
                      << config_.point_str << "'. Expected format: 'X Y Z' or 'X,Y,Z'\n";
            return 1;
        }
    } else if (!config_.input_path.empty() && std::filesystem::exists(config_.input_path)) {
        std::ifstream fin(config_.input_path);
        if (!fin.good()) {
            std::cerr << "[FreeWake Error]: Cannot open input file '" << config_.input_path << "'\n";
            return 1;
        }
        std::string firstLine;
        if (std::getline(fin, firstLine)) {
            std::stringstream ss(firstLine);
            double val1, val2;
            if (ss >> val1 && !(ss >> val2)) {
                // Line 1 was just count, read next line
                int ts_file = 0;
                if (fin >> ts_file >> P[0] >> P[1] >> P[2]) {
                    if (std::filesystem::exists(resolve_output_file("timestep" + std::to_string(ts_file) + ".txt"))) {
                        timestep = ts_file;
                    }
                }
            } else {
                std::stringstream ss2(firstLine);
                int ts_file = 0;
                if (ss2 >> ts_file >> P[0] >> P[1] >> P[2]) {
                    if (std::filesystem::exists(resolve_output_file("timestep" + std::to_string(ts_file) + ".txt"))) {
                        timestep = ts_file;
                    }
                }
            }
        }
    } else {
        std::filesystem::path defaultPointFile = std::filesystem::path(config_.output_dir) / "pointinfo.txt";
        if (std::filesystem::exists(defaultPointFile)) {
            std::ifstream fin(defaultPointFile);
            int count = 0;
            if (fin >> count && count > 0) {
                int ts = 0;
                fin >> ts >> P[0] >> P[1] >> P[2];
                timestep = ts;
            }
        }
    }

    // 2. Locate timestep file
    OUTPUT_PATH = config_.output_dir;
    std::filesystem::path timeFile = resolve_output_file("timestep" + std::to_string(timestep) + ".txt");
    if (!std::filesystem::exists(timeFile)) {
        // Fallback: search for any available timestep file in output_dir
        bool found = false;
        if (std::filesystem::exists(config_.output_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(config_.output_dir)) {
                std::string fn = entry.path().filename().string();
                if (fn.rfind("timestep", 0) == 0 && fn.find(".txt") != std::string::npos) {
                    timeFile = entry.path();
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            std::cerr << "[FreeWake Error]: Timestep file not found in '" << config_.output_dir
                      << "'. Please run 'freewake solve' first to generate simulation timesteps.\n";
            return 1;
        }
    }

    // 3. Read timestep data
    std::vector<DVE> surface(1000);
    std::vector<std::vector<DVE>> wake_storage(100, std::vector<DVE>(1000));
    std::vector<DVE*> wake(100);
    for (size_t i = 0; i < 100; ++i) {
        wake[i] = wake_storage[i].data();
    }

    Read_Timestep(timestep, surface.data(), wake.data());

    // 4. Compute induced velocity
    double w_ind[3] = {0.0, 0.0, 0.0};
    DVE_Induced_Velocity(info, P, surface.data(), wake.data(), timestep, w_ind);

    double speed = std::sqrt(w_ind[0] * w_ind[0] + w_ind[1] * w_ind[1] + w_ind[2] * w_ind[2]);

    std::cout << "===========================================================================\n";
    std::cout << " FreeWake Point Velocity Evaluation (Timestep " << timestep << ")\n";
    std::cout << "===========================================================================\n";
    std::cout << "Evaluation Point P: (" << P[0] << ", " << P[1] << ", " << P[2] << ")\n";
    std::cout << "Induced Velocity Vector: \n";
    std::cout << "  u = " << w_ind[0] << " m/s\n";
    std::cout << "  v = " << w_ind[1] << " m/s\n";
    std::cout << "  w = " << w_ind[2] << " m/s\n";
    std::cout << "Total Induced Speed |V_ind| = " << speed << " m/s\n";

    if (!config_.output_path.empty()) {
        std::ofstream fout(config_.output_path);
        if (fout.good()) {
            fout << "# FreeWake Point Velocity Evaluation\n";
            fout << "# x y z u v w |V_ind|\n";
            fout << P[0] << " " << P[1] << " " << P[2] << " "
                 << w_ind[0] << " " << w_ind[1] << " " << w_ind[2] << " " << speed << "\n";
            std::cout << "[FreeWake]: Output written to " << config_.output_path << "\n";
        }
    }

    return 0;
}

} // namespace FreeWake
