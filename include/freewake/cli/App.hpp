#pragma once

#include "freewake/commands/ICommand.hpp"
#include "freewake/commands/SolveCommand.hpp"
#include "freewake/commands/PointVelocityCommand.hpp"
#include "freewake/commands/MultiPointVelocityCommand.hpp"
#include "freewake/commands/AirfoilInfoCommand.hpp"
#include <memory>
#include <string>

namespace CLI {
class App;
}

namespace FreeWake {

class CLIApp {
public:
    CLIApp(int argc, char** argv);
    ~CLIApp();

    int run();

private:
    void setupCommands();

    int argc_;
    char** argv_;
    std::unique_ptr<CLI::App> app_;
    std::unique_ptr<ICommand> command_;

    SolveConfig solveConfig_;
    std::string fallbackConfigPath_;
    PointVelocityConfig pointVelocityConfig_;
    MultiPointVelocityConfig multiPointVelocityConfig_;
    AirfoilInfoConfig airfoilInfoConfig_;

    bool verbose_ = false;
};

} // namespace FreeWake
