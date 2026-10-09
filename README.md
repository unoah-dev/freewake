# FreeWake

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C.svg?logo=cmake)](https://cmake.org/)
[![License: GPL-2.0](https://img.shields.io/badge/License-GPL%202.0-blue.svg)](LICENSE.md)

**FreeWake** is an aerodynamic analysis and flight performance simulation suite based on a higher-order vortex-lattice formulation with Distributed Vorticity Elements (DVE) and force-free wake modeling.

---

## 1. Overview & Historical Attribution

### Theoretical Foundations
The underlying theoretical model, mathematical formulation, and numerical implementations were originally developed by **Dr. Götz Bramesfeld**:

> **Bramesfeld, Götz.** *“A Higher Order Vortex-Lattice Method with a Force-Free Wake.”*  
> Ph.D. Dissertation, Department of Aerospace Engineering, The Pennsylvania State University, August 2006.

The algorithm expands upon:
- The multiple lifting-line methodology of **K.-H. Horstmann** (*“Ein Mehrfach-Traglinienverfahren und seine Verwendung fuer Entwurf und Nachrechnung nichtplanarer Fluegelanordnungen”*, DFVLR-FB 87-51, 1987).
- Trailing-edge induced drag evaluation techniques by **R. Eppler** and **Schmid-Göller** (1990).

### Capabilities
- **Distributed Vorticity Elements (DVE)**: Lifting surfaces and shed vortex sheets feature continuous spanwise parabolic circulation distributions ($\Gamma(y) = A + By + Cy^2$), eliminating unphysical point singularities.
- **Dual Wake Solvers**:
  - *Prescribed Wake Model*: Fixed, planar/semi-infinite vortex sheets for rapid cruise and trim sweeps.
  - *Relaxed Force-Free Wake Model*: Time-stepping Lagrangian wake relaxation that sheds and advects trailing vortex elements according to locally induced Biot-Savart velocities.
- **Longitudinal Trim Iteration**: Automatically trims multi-surface aircraft configurations (e.g., wing + horizontal tail) to zero residual pitching moment about the center of gravity ($C_M = 0$).
- **Sectional Viscous Drag & Stall Modeling**: Integrates profile drag and applies non-linear stall corrections derived from 2D airfoil polar tables ($\alpha, c_l, c_d, Re, c_m$).
- **Total Drag Breakdown**: Computes induced drag ($D_i$), wing profile drag ($D_{\text{prof}}$), horizontal tail drag ($D_{\text{ht}}$), vertical tail drag ($D_{\text{vt}}$), fuselage friction drag ($D_{\text{fus}}$), and empirical interference drag ($D_{\text{int}}$).

---

## 2. Modernization Summary

The codebase on the `unoah-dev` branch has undergone comprehensive modernization from legacy monolithic C/C++ into a clean, modern **C++17** software architecture:

1. **Modular Layout & Translation Unit Decoupling**:
   - Replaced legacy unity-build includes (`#include "../src/*.cpp"`) with proper 1:1 header/source pairing.
   - Public library headers are organized under `include/freewake/` using `#pragma once` include guards.
   - Implementations are organized under `src/` and `src/commands/`.
   - Global simulation state and memory counters use C++17 `inline` semantics to guarantee ODR compliance.
2. **Modern CLI Architecture (CLI11 + Strategy Pattern)**:
   - Replaced rigid, hardcoded main loops and interactive `scanf` prompts with a modern Command Line Interface using **CLI11**.
   - Subcommand business logic is decoupled through a polymorphic strategy pattern (`FreeWake::ICommand`).
   - Domain-familiar subcommands (`solve`, `point-velocity`, `multi-point-velocity`, `airfoil-info`) provide flexible, scriptable execution.
   - Built-in backward compatibility transparently runs default simulations when invoked without subcommands.
3. **Linear Algebra Vectorization & Modern I/O**:
   - 3D vector algebra, Euler coordinate rotations, and dense linear system solves are accelerated using **Eigen3** (`Eigen::PartialPivLU`).
   - Clean YAML configuration support using **yaml-cpp**, alongside legacy input compatibility.
   - Cross-platform filesystem operations use `std::filesystem`.
   - RAII memory containers (`Array2D`, `std::vector`, `std::unique_ptr`) prevent memory leaks.

---

## 3. Prerequisites & Dependencies

### Compiler & Toolchain
- **C++ Compiler**: Any modern C++17 compliant compiler:
  - GCC 9 or newer
  - Clang 10 or newer (Apple Clang 12+)
  - Microsoft Visual Studio 2019 or newer (MSVC)
- **Build System**: [CMake](https://cmake.org/) version 3.16 or later.

### Third-Party Dependencies
All third-party libraries are integrated via CMake's `FetchContent` and downloaded/configured automatically during the build process. **No manual installation is required**:

| Library | Version | Purpose |
|---|---|---|
| **[CLI11](https://github.com/CLIUtils/CLI11)** | v2.4.1 | Command-line argument parsing and subcommand dispatch |
| **[Eigen3](https://gitlab.com/libeigen/eigen)** | 3.4.0 | Vectorized 3D geometry transformations and linear system solver |
| **[yaml-cpp](https://github.com/jbeder/yaml-cpp)** | 0.8.0 | Structured simulation configuration parser |

---

## 4. Build Instructions (Cross-Platform)

### Linux & macOS
```bash
# Clone the repository
git clone https://github.com/unoah-dev/freewake.git
cd freewake

# Configure and build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# The binary is generated in build/bin/FreeWake
./bin/FreeWake --version
```

### Windows (PowerShell / Visual Studio MSVC)
```powershell
# Clone the repository
git clone https://github.com/unoah-dev/freewake.git
cd freewake

# Configure and build in Release mode
mkdir build
cd build
cmake ..
cmake --build . --config Release

# The binary is generated in bin/Release/FreeWake.exe (or bin/FreeWake.exe)
.\bin\Release\FreeWake.exe --version
```

---

## 5. CLI Subcommand Usage & Capabilities

FreeWake features a modern subcommand-driven command line interface.

```
Usage: FreeWake [OPTIONS] [config_file] [SUBCOMMAND]
```

### Global Options
- `-h, --help`: Display help information for any command or subcommand.
- `-v, --version`: Display program version and build details.
- `--verbose`: Enable detailed solver diagnostics during execution.

### Subcommands

#### 1. `solve` — Aerodynamic Simulation Run
Executes the angle-of-attack sweep, longitudinal trim iteration, profile drag lookups, and total force integration.

```bash
# Run simulation using modern YAML config
./build/bin/FreeWake solve -c input.yaml -o output/

# Run simulation using legacy text input
./build/bin/FreeWake solve -c input.txt -o output/
```

- **Options**:
  - `-c, --config <path>`: Path to simulation configuration file (defaults to `input.yaml` or `input.txt`).
  - `-o, --output-dir <path>`: Target output directory for solution files (default: `output/`).
  - `--verbose`: Print real-time progress for each angle of attack.

#### 2. `point-velocity` — Single-Point Velocity Evaluation
Evaluates the local induced velocity vector $\vec{w}_{\text{ind}} = (u, v, w)$ and total velocity magnitude $|V_{\text{ind}}|$ at a designated 3D coordinate.

```bash
# Evaluate velocity at explicit coordinates (X Y Z)
./build/bin/FreeWake point-velocity -p "0.5 0.0 0.1" -t 20

# Evaluate velocity at point specified in a pointinfo file
./build/bin/FreeWake point-velocity -i output/pointinfo.txt -t 20 -o output/velocity_pt.txt
```

- **Options**:
  - `-p, --point <coords>`: Target 3D coordinates formatted as `"X Y Z"` or `"X,Y,Z"`.
  - `-i, --input <path>`: Input file containing sample coordinates (e.g. `pointinfo.txt`).
  - `-t, --timestep <int>`: Simulation timestep file to evaluate (default: `20`).
  - `-o, --output <path>`: Optional file path to store calculated velocity data.
  - `--output-dir <path>`: Directory containing simulation timestep files (default: `output/`).

#### 3. `multi-point-velocity` — Grid / Multi-Point Field Evaluation
Evaluates flow-field velocities across a generated 3D grid or coordinate list, replacing legacy utility executables (`Main_MultiPointVelocity.exe`).

```bash
# Evaluate velocity across a 3D bounding box grid (xmin:xmax:nx,ymin:ymax:ny,zmin:zmax:nz)
./build/bin/FreeWake multi-point-velocity -g "-0.5:0.5:5,-0.5:0.5:5,0:0.2:3" -t 20 -o output/grid_vel.txt

# Evaluate velocity across a batch of query points from file
./build/bin/FreeWake multi-point-velocity -i output/pointinfo.txt -t 20 -o output/velocityinfo.txt
```

- **Options**:
  - `-g, --grid <spec>`: 3D grid format `"xmin:xmax:nx,ymin:ymax:ny,zmin:zmax:nz"`.
  - `-i, --input <path>`: Point coordinate list file (e.g. `pointinfo.txt`).
  - `-t, --timestep <int>`: Simulation timestep file to evaluate (default: `20`).
  - `-o, --output <path>`: Output tabular file path (default: `output/velocityinfo.txt`).
  - `--output-dir <path>`: Directory containing simulation timestep files (default: `output/`).

#### 4. `airfoil-info` — 2D Airfoil Polar Inspection & Validation
Parses, analyzes, and validates aerodynamic polar tables (`.dat`) stored in the `airfoils/` directory.

```bash
# Inspect and validate a specific airfoil polar file
./build/bin/FreeWake airfoil-info -f airfoils/airfoil1.dat

# Scan and validate all airfoil files in the default airfoils directory
./build/bin/FreeWake airfoil-info
```

- **Options**:
  - `-f, --file <path>`: Path to a specific 2D airfoil polar file.
  - `-d, --dir <path>`: Directory containing airfoil files to scan (default: `airfoils/`).

### Backward Compatibility Note
To ensure legacy scripts and continuous integration pipelines remain fully functional without modification, executing the binary without a subcommand automatically falls back to the `solve` driver:

```bash
# Automatically runs SolveCommand with default input configuration
./build/bin/FreeWake

# Automatically runs SolveCommand using the supplied positional configuration
./build/bin/FreeWake input.yaml
./build/bin/FreeWake input.txt
```


---

## 6. Output Artifacts

When executing simulations via `freewake solve`, results are written into the designated output directory (default: `output/`):

- **`Performance.txt`**: Consolidated aerodynamic performance polar across the angle-of-attack sweep ($\alpha, V_\infty, C_L, C_D, D_{\text{total}}, L/D, w_{\text{glide}}, P_{\text{req}}$) and full drag breakdown ($D_i, D_{\text{prof}}, D_{\text{ht}}, D_{\text{vt}}, D_{\text{fus}}, D_{\text{int}}$).
- **`TrimSol.txt`**: Detailed convergence log of horizontal-tail trim incidence iterations and residual moment coefficients.
- **`AOA<deg>.txt`**: Spanwise lift and drag distributions for each simulated angle of attack.
- **`timestep<N>.txt`**: DVE surface and trailing wake geometry coordinates, circulation coefficients, and induced velocities.

---

## 7. Automated Multi-Platform Builds (CI/CD Note)

> **Note on Automated Testing & CI/CD**:  
> Automated multi-platform builds and continuous integration (via **GitHub Actions**) across Linux (`ubuntu-latest`), macOS (`macos-latest`), and Windows (`windows-latest`) are planned for integration in an upcoming update to ensure cross-platform build stability and regression testing.

---

## 8. License & References

### License
This software is distributed under the terms of the **GNU General Public License v2.0 (GPL-2.0)**. See [LICENSE.md](LICENSE.md) for full license details.

### Academic References
1. Bramesfeld, G., *“A Higher Order Vortex-Lattice Method with a Force-Free Wake,”* Ph.D. Dissertation, Department of Aerospace Engineering, The Pennsylvania State University, August 2006.
2. Horstmann, K.-H., *“Ein Mehrfach-Traglinienverfahren und seine Verwendung fuer Entwurf und Nachrechnung nichtplanarer Fluegelanordnungen,”* DFVLR-FB 87-51, 1987.
3. Eppler, R., and Schmid-Göller, *“A Method to Calculate the Influence of Vortex Roll-Up on the Induced Drag of Wings,”* Finite Approximations in Fluid Mechanics II, Notes on Numerical Fluid Mechanics, Vol. 25, Vieweg, 1990.

