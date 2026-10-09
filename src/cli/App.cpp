#include "freewake/cli/App.hpp"
#include "freewake/general.hpp"
#include <CLI/CLI.hpp>
#include <iostream>

namespace FreeWake {

CLIApp::CLIApp(int argc, char** argv)
    : argc_(argc), argv_(argv), app_(std::make_unique<CLI::App>("FreeWake: Potential Flow & Free-Wake Simulation Suite"))
{
    setupCommands();
}

CLIApp::~CLIApp() = default;

void CLIApp::setupCommands()
{
    app_->set_version_flag("-v,--version", std::string(PROGRAM_VERSION) + " (CLI11 Modular Architecture)");
    app_->add_flag("--verbose", verbose_, "Enable verbose logging across operations");

    // Optional legacy positional config file
    app_->add_option("config_file", fallbackConfigPath_, "Legacy input configuration file (default fallback)");

    // Subcommand: solve
    auto* solveCmd = app_->add_subcommand("solve", "Run standard potential flow and free-wake simulation");
    solveCmd->add_option("-c,--config", solveConfig_.config_path, "Path to configuration file")
        ->default_str("input.yaml");
    solveCmd->add_option("-o,--output-dir", solveConfig_.output_dir, "Path to output directory")
        ->default_str("output");
    solveCmd->add_flag("--verbose", solveConfig_.verbose, "Enable verbose output");
    solveCmd->callback([this]() {
        command_ = std::make_unique<SolveCommand>(solveConfig_);
    });

    // Subcommand: point-velocity
    auto* pointCmd = app_->add_subcommand("point-velocity", "Evaluate flow-field velocity at an individual sample point");
    pointCmd->add_option("-i,--input", pointVelocityConfig_.input_path, "Input point file (e.g. pointinfo.txt)");
    pointCmd->add_option("-p,--point", pointVelocityConfig_.point_str, "Point coordinates 'X Y Z' or 'X,Y,Z'");
    pointCmd->add_option("-o,--output", pointVelocityConfig_.output_path, "Output velocity file path");
    pointCmd->add_option("-t,--timestep", pointVelocityConfig_.timestep, "Simulation timestep index")
        ->default_val(20);
    pointCmd->add_option("--output-dir", pointVelocityConfig_.output_dir, "Directory containing timestep files")
        ->default_str("output");
    pointCmd->callback([this]() {
        command_ = std::make_unique<PointVelocityCommand>(pointVelocityConfig_);
    });

    // Subcommand: multi-point-velocity
    auto* multiPointCmd = app_->add_subcommand("multi-point-velocity", "Evaluate flow-field velocities across a grid or list of points");
    multiPointCmd->add_option("-i,--input", multiPointVelocityConfig_.input_path, "Input point list file (e.g. pointinfo.txt)");
    multiPointCmd->add_option("-g,--grid", multiPointVelocityConfig_.grid_str, "3D grid specification 'xmin:xmax:nx,ymin:ymax:ny,zmin:zmax:nz'");
    multiPointCmd->add_option("-o,--output", multiPointVelocityConfig_.output_path, "Output velocity file path")
        ->default_str("output/velocityinfo.txt");
    multiPointCmd->add_option("-t,--timestep", multiPointVelocityConfig_.timestep, "Simulation timestep index")
        ->default_val(20);
    multiPointCmd->add_option("--output-dir", multiPointVelocityConfig_.output_dir, "Directory containing timestep files")
        ->default_str("output");
    multiPointCmd->callback([this]() {
        command_ = std::make_unique<MultiPointVelocityCommand>(multiPointVelocityConfig_);
    });

    // Subcommand: airfoil-info
    auto* airfoilCmd = app_->add_subcommand("airfoil-info", "Inspect and validate 2D airfoil polar files");
    airfoilCmd->add_option("-f,--file", airfoilInfoConfig_.file_path, "Path to specific airfoil polar file (.dat)");
    airfoilCmd->add_option("-d,--dir", airfoilInfoConfig_.airfoils_dir, "Directory of airfoil polar files")
        ->default_str("airfoils");
    airfoilCmd->callback([this]() {
        command_ = std::make_unique<AirfoilInfoCommand>(airfoilInfoConfig_);
    });
}

int CLIApp::run()
{
    try {
        app_->parse(argc_, argv_);
    } catch (const CLI::CallForHelp& e) {
        return app_->exit(e);
    } catch (const CLI::CallForVersion& e) {
        return app_->exit(e);
    } catch (const CLI::ParseError& e) {
        return app_->exit(e);
    }

    // Backward compatibility: If no subcommand was parsed, fallback to SolveCommand
    if (!command_) {
        if (!fallbackConfigPath_.empty()) {
            solveConfig_.config_path = fallbackConfigPath_;
        }
        solveConfig_.verbose = verbose_;
        command_ = std::make_unique<SolveCommand>(solveConfig_);
    }

    return command_->execute();
}

} // namespace FreeWake
