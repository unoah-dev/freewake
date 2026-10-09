#pragma once

#include "freewake/commands/ICommand.hpp"
#include <string>

namespace FreeWake {

struct PointVelocityConfig {
    std::string input_path;     // path to point file (e.g. pointinfo.txt)
    std::string point_str;      // e.g. "0.5,0.0,0.1" or "0.5 0.0 0.1"
    std::string output_path;    // path to output file (default: stdout or velocityinfo.txt)
    int timestep = 20;          // timestep index to evaluate
    std::string output_dir = "output"; // directory containing timestep files
    bool verbose = false;
};

class PointVelocityCommand : public ICommand {
public:
    explicit PointVelocityCommand(PointVelocityConfig config);
    int execute() override;

private:
    PointVelocityConfig config_;
};

} // namespace FreeWake
