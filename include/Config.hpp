#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "typedef.h"
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <stdexcept>

struct SimulationConfig {
    bool relaxed_wake = false;
    int aerodynamics = 1; // 1 = steady, 2 = unsteady
    bool viscous = true;
    bool symmetrical = false;
    bool longitudinal_trim = false;
    int max_time_steps = 20;
    double time_step_size = 2.5;
    double convergence_delta_e = 0.0;
};

struct FlowConditionsConfig {
    double freestream_velocity = 11.0;
    double alpha_start_deg = 0.0;
    double alpha_end_deg = 10.0;
    double alpha_step_deg = 2.0;
    double sideslip_angle_deg = 0.0;
    double density = 1000.0;
    double kinematic_viscosity = 1.307e-06;
};

struct ReferenceConfig {
    double area = 0.0885;
    double span = 0.6;
    double mean_aerodynamic_chord = 0.1;
    double aircraft_weight = 801.0;
    std::array<double, 3> cg_location = {0.0, 0.0, 0.0};
    double wing_cmo = -0.1;
};

struct PanelEdgeConfig {
    std::array<double, 3> coordinates = {0.0, 0.0, 0.0};
    double chord = 0.0;
    double incidence_deg = 0.0;
    int boundary_condition = 0;
    int airfoil = 1;
};

struct PanelConfig {
    int id = 1;
    int num_spanwise_elements = 3;
    int neighbor_left = 0;
    int neighbor_right = 0;
    PanelEdgeConfig left_edge;
    PanelEdgeConfig right_edge;
};

struct VerticalTailPanelConfig {
    int id = 1;
    double chord = 0.0;
    double area = 0.0;
    int airfoil = 1;
};

struct VerticalTailConfig {
    std::vector<VerticalTailPanelConfig> panels;
};

struct FuselageConfig {
    double section_width = 0.0;
    int transition_panel = 0;
    std::vector<double> diameters;
};

struct DragConfig {
    double interference_drag_percent = 0.0;
};

struct AirfoilConfig {
    int count = 8;
    std::string directory = "airfoils/";
    std::vector<std::string> files;
};

struct Config {
    SimulationConfig simulation;
    FlowConditionsConfig flow;
    ReferenceConfig reference;
    int num_wings = 2;
    int num_chordwise_lifting_lines = 3;
    std::vector<PanelConfig> panels;
    VerticalTailConfig vertical_tail;
    FuselageConfig fuselage;
    DragConfig drag;
    AirfoilConfig airfoils;

    // Load configuration from a YAML file
    static Config loadFromFile(const std::string& filepath);

    // Apply configuration into legacy solver structures
    void apply(GENERAL& info, double& alpha1, double& alpha2, double& alphastep,
               std::vector<PANEL>& panel_vec,
               double VTchord[5], double VTarea[5], int VTairfoil[5],
               double FusSectS[20], double& delFus, int& FusLT, double& IFdrag) const;

    // Get path to an airfoil data file (0-based index)
    std::string getAirfoilFilePath(int airfoil_index_0based) const;
};

#endif // CONFIG_HPP
