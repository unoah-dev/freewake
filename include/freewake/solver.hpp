#pragma once

#include <string>

namespace FreeWake {

int run_simulation(const std::string& configFile = "input.yaml",
                   const std::string& outputDir = "output",
                   bool verbose = false);

} // namespace FreeWake
