#pragma once

#include "freewake/commands/ICommand.hpp"
#include <string>

namespace FreeWake {

struct SolveConfig {
    std::string config_path = "input.yaml";
    std::string output_dir = "output";
    bool verbose = false;
};

class SolveCommand : public ICommand {
public:
    explicit SolveCommand(SolveConfig config);
    int execute() override;

private:
    SolveConfig config_;
};

} // namespace FreeWake
