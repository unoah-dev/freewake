#include "freewake/commands/MultiPointVelocityCommand.hpp"
#include "freewake/general.hpp"
#include "freewake/induced_velocity.hpp"
#include "freewake/read_input.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <array>
#include <cmath>

namespace FreeWake {

MultiPointVelocityCommand::MultiPointVelocityCommand(MultiPointVelocityConfig config)
    : config_(std::move(config))
{
}

int MultiPointVelocityCommand::execute()
{
    std::vector<std::array<double, 3>> points;
    int timestep = config_.timestep;

    // 1. Generate points from grid or read from input file
    if (!config_.grid_str.empty()) {
        double xmin = -1.0, xmax = 1.0; int nx = 5;
        double ymin = -1.0, ymax = 1.0; int ny = 5;
        double zmin = -0.5, zmax = 0.5; int nz = 3;

        std::string s = config_.grid_str;
        for (char& c : s) {
            if (c == ':' || c == ',') c = ' ';
        }
        std::stringstream ss(s);
        ss >> xmin >> xmax >> nx >> ymin >> ymax >> ny >> zmin >> zmax >> nz;

        nx = std::max(1, nx); ny = std::max(1, ny); nz = std::max(1, nz);
        double dx = (nx > 1) ? (xmax - xmin) / (nx - 1) : 0.0;
        double dy = (ny > 1) ? (ymax - ymin) / (ny - 1) : 0.0;
        double dz = (nz > 1) ? (zmax - zmin) / (nz - 1) : 0.0;

        for (int ix = 0; ix < nx; ++ix) {
            double x = xmin + ix * dx;
            for (int iy = 0; iy < ny; ++iy) {
                double y = ymin + iy * dy;
                for (int iz = 0; iz < nz; ++iz) {
                    points.push_back({x, y, zmin + iz * dz});
                }
            }
        }
        std::cout << "[FreeWake]: Generated " << points.size() << " grid evaluation points.\n";
    } else {
        std::string inputFile = config_.input_path;
        if (inputFile.empty()) {
            std::filesystem::path defaultFile = std::filesystem::path(config_.output_dir) / "pointinfo.txt";
            if (std::filesystem::exists(defaultFile)) inputFile = defaultFile.string();
        }

        if (!inputFile.empty() && std::filesystem::exists(inputFile)) {
            std::ifstream fin(inputFile);
            int count = 0;
            if (fin >> count && count > 0) {
                for (int i = 0; i < count; ++i) {
                    int ts = 0; double x = 0.0, y = 0.0, z = 0.0;
                    if (fin >> ts >> x >> y >> z) {
                        points.push_back({x, y, z});
                        if (i == 0 && std::filesystem::exists(resolve_output_file("timestep" + std::to_string(ts) + ".txt"))) {
                            timestep = ts;
                        }
                    }
                }
            } else {
                fin.clear(); fin.seekg(0);
                double x = 0.0, y = 0.0, z = 0.0;
                while (fin >> x >> y >> z) points.push_back({x, y, z});
            }
            std::cout << "[FreeWake]: Read " << points.size() << " points from '" << inputFile << "'.\n";
        } else {
            for (int i = -5; i <= 5; ++i) points.push_back({i * 0.1, 0.0, 0.0});
            std::cout << "[FreeWake]: Using default line of " << points.size() << " points.\n";
        }
    }

    if (points.empty()) {
        std::cerr << "[FreeWake Error]: No points to evaluate.\n";
        return 1;
    }


    // 2. Locate timestep file
    OUTPUT_PATH = config_.output_dir;
    std::filesystem::path timeFile = resolve_output_file("timestep" + std::to_string(timestep) + ".txt");
    if (!std::filesystem::exists(timeFile)) {
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
                      << "'. Run 'freewake solve' first to generate simulation timesteps.\n";
            return 1;
        }
    }

    // 3. Read timestep data
    std::vector<DVE> surface(1000);
    std::vector<std::vector<DVE>> wake_storage(100, std::vector<DVE>(1000));
    std::vector<DVE*> wake(100);
    for (size_t i = 0; i < 100; ++i) wake[i] = wake_storage[i].data();

    Read_Timestep(timestep, surface.data(), wake.data());

    // 4. Compute velocities and write output
    std::string outPath = config_.output_path;
    if (outPath.empty()) {
        outPath = (std::filesystem::path(config_.output_dir) / "velocityinfo.txt").string();
    }
    std::filesystem::create_directories(std::filesystem::path(outPath).parent_path());
    std::ofstream fout(outPath);
    if (!fout.good()) {
        std::cerr << "[FreeWake Error]: Cannot open output file '" << outPath << "'\n";
        return 1;
    }

    fout << "# FreeWake Multi-Point Velocity Evaluation (Timestep " << timestep << ")\n";
    fout << "# Total Points: " << points.size() << "\n";
    fout << "# x y z u v w |V_ind|\n";

    for (size_t i = 0; i < points.size(); ++i) {
        double P[3] = {points[i][0], points[i][1], points[i][2]};
        double w_ind[3] = {0.0, 0.0, 0.0};
        DVE_Induced_Velocity(info, P, surface.data(), wake.data(), timestep, w_ind);
        double speed = std::sqrt(w_ind[0] * w_ind[0] + w_ind[1] * w_ind[1] + w_ind[2] * w_ind[2]);

        char buf[256];
        std::snprintf(buf, sizeof(buf), "%15.6f %15.6f %15.6f %15.6f %15.6f %15.6f %15.6f\n",
                      P[0], P[1], P[2], w_ind[0], w_ind[1], w_ind[2], speed);
        fout << buf;
    }

    std::cout << "[FreeWake]: Evaluated " << points.size() << " velocity points. Results written to: "
              << outPath << "\n";
    return 0;
}

} // namespace FreeWake
