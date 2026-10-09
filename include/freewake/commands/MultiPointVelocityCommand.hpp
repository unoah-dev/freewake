#pragma once

#include "freewake/commands/ICommand.hpp"
#include <string>

namespace FreeWake {

struct MultiPointVelocityConfig {
    std::string input_path;     // path to file containing point list (e.g. pointinfo.txt)
    std::string grid_str;       // grid definition e.g. "xmin:xmax:nx,ymin:ymax:ny,zmin:zmax:nz"
    std::string output_path = "output/velocityinfo.txt";
    int timestep = 20;
    std::string output_dir = "output";
    bool verbose = false;
};

class MultiPointVelocityCommand : public ICommand {
public:
    explicit MultiPointVelocityCommand(MultiPointVelocityConfig config);
    int execute() override;

private:
    MultiPointVelocityConfig config_;
};

} // namespace FreeWake
