#pragma once

#include "freewake/commands/ICommand.hpp"
#include <string>

namespace FreeWake {

struct AirfoilInfoConfig {
    std::string file_path;
    std::string airfoils_dir = "airfoils";
    bool verbose = false;
};

class AirfoilInfoCommand : public ICommand {
public:
    explicit AirfoilInfoCommand(AirfoilInfoConfig config);
    int execute() override;

private:
    int inspectFile(const std::string& path);
    AirfoilInfoConfig config_;
};

} // namespace FreeWake
