#include "freewake/commands/SolveCommand.hpp"
#include "freewake/solver.hpp"
#include <iostream>
#include <utility>

namespace FreeWake {

SolveCommand::SolveCommand(SolveConfig config)
    : config_(std::move(config))
{
}

int SolveCommand::execute()
{
    return run_simulation(config_.config_path, config_.output_dir, config_.verbose);
}

} // namespace FreeWake
