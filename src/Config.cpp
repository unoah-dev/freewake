#include "Config.hpp"
#include "typedef.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>

namespace {

constexpr double Pi = 3.1415926535897931;
constexpr double DtR = Pi / 180.0;

template <typename T>
T getRequired(const YAML::Node& node, const std::string& key, const std::string& context) {
    if (!node[key]) {
        throw std::runtime_error("Configuration error: missing required key '" + key + "' in " + context);
    }
    try {
        return node[key].as<T>();
    } catch (const YAML::BadConversion& e) {
        throw std::runtime_error("Configuration error: invalid value/type for key '" + key + "' in " + context + ": " + e.what());
    }
}

template <typename T>
T getOptional(const YAML::Node& node, const std::string& key, const T& defaultValue) {
    if (!node[key]) {
        return defaultValue;
    }
    try {
        return node[key].as<T>();
    } catch (const YAML::BadConversion&) {
        return defaultValue;
    }
}

} // anonymous namespace

Config Config::loadFromFile(const std::string& filepath) {
    std::ifstream fin(filepath);
    if (!fin.good()) {
        throw std::runtime_error("Configuration error: cannot open input file '" + filepath + "'");
    }

    YAML::Node root;
    try {
        root = YAML::Load(fin);
    } catch (const YAML::ParserException& e) {
        throw std::runtime_error("Configuration error: YAML syntax error in '" + filepath + "': " + e.what());
    } catch (const std::exception& e) {
        throw std::runtime_error("Configuration error: failed to parse YAML in '" + filepath + "': " + e.what());
    }

    Config cfg;

    // 1. Simulation settings
    if (!root["simulation"]) {
        throw std::runtime_error("Configuration error: missing required section 'simulation' in '" + filepath + "'");
    }
    const YAML::Node& simNode = root["simulation"];
    cfg.simulation.relaxed_wake = getRequired<bool>(simNode, "relaxed_wake", "simulation");
    cfg.simulation.aerodynamics = getRequired<int>(simNode, "aerodynamics", "simulation");
    cfg.simulation.viscous = getRequired<bool>(simNode, "viscous", "simulation");
    cfg.simulation.symmetrical = getRequired<bool>(simNode, "symmetrical", "simulation");
    cfg.simulation.longitudinal_trim = getRequired<bool>(simNode, "longitudinal_trim", "simulation");
    cfg.simulation.max_time_steps = getRequired<int>(simNode, "max_time_steps", "simulation");
    cfg.simulation.time_step_size = getRequired<double>(simNode, "time_step_size", "simulation");
    cfg.simulation.convergence_delta_e = getRequired<double>(simNode, "convergence_delta_e", "simulation");

    // 2. Flow conditions
    if (!root["flow_conditions"]) {
        throw std::runtime_error("Configuration error: missing required section 'flow_conditions' in '" + filepath + "'");
    }
    const YAML::Node& flowNode = root["flow_conditions"];
    cfg.flow.freestream_velocity = getRequired<double>(flowNode, "freestream_velocity", "flow_conditions");
    if (!flowNode["alpha_sweep"]) {
        throw std::runtime_error("Configuration error: missing required key 'alpha_sweep' in flow_conditions");
    }
    const YAML::Node& alphaNode = flowNode["alpha_sweep"];
    cfg.flow.alpha_start_deg = getRequired<double>(alphaNode, "start_deg", "flow_conditions.alpha_sweep");
    cfg.flow.alpha_end_deg = getRequired<double>(alphaNode, "end_deg", "flow_conditions.alpha_sweep");
    cfg.flow.alpha_step_deg = getRequired<double>(alphaNode, "step_deg", "flow_conditions.alpha_sweep");
    cfg.flow.sideslip_angle_deg = getRequired<double>(flowNode, "sideslip_angle_deg", "flow_conditions");
    cfg.flow.density = getRequired<double>(flowNode, "density", "flow_conditions");
    cfg.flow.kinematic_viscosity = getRequired<double>(flowNode, "kinematic_viscosity", "flow_conditions");

    // 3. Reference geometry
    if (!root["reference"]) {
        throw std::runtime_error("Configuration error: missing required section 'reference' in '" + filepath + "'");
    }
    const YAML::Node& refNode = root["reference"];
    cfg.reference.area = getRequired<double>(refNode, "area", "reference");
    cfg.reference.span = getRequired<double>(refNode, "span", "reference");
    cfg.reference.mean_aerodynamic_chord = getRequired<double>(refNode, "mean_aerodynamic_chord", "reference");
    cfg.reference.aircraft_weight = getRequired<double>(refNode, "aircraft_weight", "reference");
    if (!refNode["cg_location"] || !refNode["cg_location"].IsSequence() || refNode["cg_location"].size() < 3) {
        throw std::runtime_error("Configuration error: 'reference.cg_location' must be a sequence of 3 numbers [x, y, z]");
    }
    cfg.reference.cg_location[0] = refNode["cg_location"][0].as<double>();
    cfg.reference.cg_location[1] = refNode["cg_location"][1].as<double>();
    cfg.reference.cg_location[2] = refNode["cg_location"][2].as<double>();
    cfg.reference.wing_cmo = getRequired<double>(refNode, "wing_cmo", "reference");

    // 4. Geometry and Paneling
    if (!root["geometry"]) {
        throw std::runtime_error("Configuration error: missing required section 'geometry' in '" + filepath + "'");
    }
    const YAML::Node& geomNode = root["geometry"];
    cfg.num_wings = getRequired<int>(geomNode, "num_wings", "geometry");
    cfg.num_chordwise_lifting_lines = getRequired<int>(geomNode, "num_chordwise_lifting_lines", "geometry");

    if (!geomNode["panels"] || !geomNode["panels"].IsSequence()) {
        throw std::runtime_error("Configuration error: 'geometry.panels' must be a non-empty sequence of panels");
    }
    for (size_t i = 0; i < geomNode["panels"].size(); ++i) {
        const YAML::Node& pNode = geomNode["panels"][i];
        std::string pCtx = "geometry.panels[" + std::to_string(i) + "]";

        PanelConfig p;
        p.id = getOptional<int>(pNode, "id", static_cast<int>(i + 1));
        p.num_spanwise_elements = getRequired<int>(pNode, "num_spanwise_elements", pCtx);

        if (!pNode["neighbors"]) {
            throw std::runtime_error("Configuration error: missing 'neighbors' in " + pCtx);
        }
        p.neighbor_left = getRequired<int>(pNode["neighbors"], "left", pCtx + ".neighbors");
        p.neighbor_right = getRequired<int>(pNode["neighbors"], "right", pCtx + ".neighbors");

        // Left edge
        if (!pNode["left_edge"]) {
            throw std::runtime_error("Configuration error: missing 'left_edge' in " + pCtx);
        }
        const YAML::Node& leNode = pNode["left_edge"];
        std::string leCtx = pCtx + ".left_edge";
        if (!leNode["coordinates"] || !leNode["coordinates"].IsSequence() || leNode["coordinates"].size() < 3) {
            throw std::runtime_error("Configuration error: 'coordinates' in " + leCtx + " must be a sequence of 3 numbers");
        }
        p.left_edge.coordinates[0] = leNode["coordinates"][0].as<double>();
        p.left_edge.coordinates[1] = leNode["coordinates"][1].as<double>();
        p.left_edge.coordinates[2] = leNode["coordinates"][2].as<double>();
        p.left_edge.chord = getRequired<double>(leNode, "chord", leCtx);
        p.left_edge.incidence_deg = getRequired<double>(leNode, "incidence_deg", leCtx);
        p.left_edge.boundary_condition = getRequired<int>(leNode, "boundary_condition", leCtx);
        p.left_edge.airfoil = getRequired<int>(leNode, "airfoil", leCtx);

        // Right edge
        if (!pNode["right_edge"]) {
            throw std::runtime_error("Configuration error: missing 'right_edge' in " + pCtx);
        }
        const YAML::Node& reNode = pNode["right_edge"];
        std::string reCtx = pCtx + ".right_edge";
        if (!reNode["coordinates"] || !reNode["coordinates"].IsSequence() || reNode["coordinates"].size() < 3) {
            throw std::runtime_error("Configuration error: 'coordinates' in " + reCtx + " must be a sequence of 3 numbers");
        }
        p.right_edge.coordinates[0] = reNode["coordinates"][0].as<double>();
        p.right_edge.coordinates[1] = reNode["coordinates"][1].as<double>();
        p.right_edge.coordinates[2] = reNode["coordinates"][2].as<double>();
        p.right_edge.chord = getRequired<double>(reNode, "chord", reCtx);
        p.right_edge.incidence_deg = getRequired<double>(reNode, "incidence_deg", reCtx);
        p.right_edge.boundary_condition = getRequired<int>(reNode, "boundary_condition", reCtx);
        p.right_edge.airfoil = getRequired<int>(reNode, "airfoil", reCtx);

        cfg.panels.push_back(p);
    }

    // 5. Vertical Tail (optional)
    if (root["vertical_tail"] && root["vertical_tail"]["panels"]) {
        const YAML::Node& vtNode = root["vertical_tail"]["panels"];
        if (vtNode.IsSequence()) {
            for (size_t i = 0; i < vtNode.size(); ++i) {
                VerticalTailPanelConfig vtp;
                vtp.id = getOptional<int>(vtNode[i], "id", static_cast<int>(i + 1));
                vtp.chord = getRequired<double>(vtNode[i], "chord", "vertical_tail.panels[" + std::to_string(i) + "]");
                vtp.area = getRequired<double>(vtNode[i], "area", "vertical_tail.panels[" + std::to_string(i) + "]");
                vtp.airfoil = getRequired<int>(vtNode[i], "airfoil", "vertical_tail.panels[" + std::to_string(i) + "]");
                cfg.vertical_tail.panels.push_back(vtp);
            }
        }
    }

    // 6. Fuselage (optional)
    if (root["fuselage"]) {
        const YAML::Node& fusNode = root["fuselage"];
        cfg.fuselage.section_width = getOptional<double>(fusNode, "section_width", 0.0);
        cfg.fuselage.transition_panel = getOptional<int>(fusNode, "transition_panel", 0);
        if (fusNode["sections"] && fusNode["sections"].IsSequence()) {
            for (size_t i = 0; i < fusNode["sections"].size(); ++i) {
                cfg.fuselage.diameters.push_back(fusNode["sections"][i].as<double>());
            }
        }
    }

    // 7. Drag (optional)
    if (root["drag"]) {
        cfg.drag.interference_drag_percent = getOptional<double>(root["drag"], "interference_drag_percent", 0.0);
    } else if (root["interference_drag_percent"]) {
        cfg.drag.interference_drag_percent = root["interference_drag_percent"].as<double>();
    }

    // 8. Airfoils
    if (root["airfoils"]) {
        const YAML::Node& afNode = root["airfoils"];
        cfg.airfoils.count = getOptional<int>(afNode, "count", 8);
        cfg.airfoils.directory = getOptional<std::string>(afNode, "directory", "airfoils/");
        if (afNode["files"] && afNode["files"].IsSequence()) {
            for (size_t i = 0; i < afNode["files"].size(); ++i) {
                cfg.airfoils.files.push_back(afNode["files"][i].as<std::string>());
            }
        }
    }

    return cfg;
}

void Config::apply(GENERAL& info, double& alpha1, double& alpha2, double& alphastep,
                   std::vector<PANEL>& panel_vec,
                   double VTchord[5], double VTarea[5], int VTairfoil[5],
                   double FusSectS[20], double& delFus, int& FusLT, double& IFdrag) const {
    // 1. Simulation settings
    info.relax = simulation.relaxed_wake ? 1 : 0;
    info.steady = simulation.aerodynamics;
    info.flagVISCOUS = simulation.viscous;
    info.sym = simulation.symmetrical ? 1 : 0;
    info.linear = 0;
    info.trim = simulation.longitudinal_trim ? 1 : 0;
    info.maxtime = simulation.max_time_steps;
    info.deltime = simulation.time_step_size;
    info.deltae = simulation.convergence_delta_e * simulation.convergence_delta_e;

    // 2. Flow conditions
    info.Uinf = flow.freestream_velocity;
    alpha1 = flow.alpha_start_deg * DtR;
    alpha2 = flow.alpha_end_deg * DtR;
    alphastep = flow.alpha_step_deg * DtR;
    info.beta = flow.sideslip_angle_deg * DtR;

    info.U[0] = info.Uinf * cos(alpha1) * cos(info.beta);
    info.U[1] = info.Uinf * sin(info.beta);
    info.U[2] = info.Uinf * sin(alpha1) * cos(info.beta);

    info.density = flow.density;
    info.nu = flow.kinematic_viscosity;

    // 3. Reference geometry
    info.S = reference.area;
    info.b = reference.span;
    info.cmac = reference.mean_aerodynamic_chord;
    info.W = reference.aircraft_weight;
    info.RefPt[0] = reference.cg_location[0];
    info.RefPt[1] = reference.cg_location[1];
    info.RefPt[2] = reference.cg_location[2];
    info.CMoWing = reference.wing_cmo;

    // 4. Panel dimensions and counts
    info.nowing = num_wings;
    info.nopanel = static_cast<int>(panels.size());
    info.m = num_chordwise_lifting_lines;
    info.noairfoils = airfoils.count;
    info.AR = info.b * info.b / info.S;

    // 5. Panels
    panel_vec.resize(info.nopanel);
    for (size_t i = 0; i < panels.size(); ++i) {
        const auto& p = panels[i];
        panel_vec[i].n = p.num_spanwise_elements;
        panel_vec[i].airfoil = -1;
        panel_vec[i].left = p.neighbor_left;
        panel_vec[i].right = p.neighbor_right;

        panel_vec[i].x1[0] = p.left_edge.coordinates[0];
        panel_vec[i].x1[1] = p.left_edge.coordinates[1];
        panel_vec[i].x1[2] = p.left_edge.coordinates[2];
        panel_vec[i].c1 = p.left_edge.chord;
        panel_vec[i].eps1 = p.left_edge.incidence_deg * DtR;
        panel_vec[i].BC1 = p.left_edge.boundary_condition;
        panel_vec[i].airfoil1 = p.left_edge.airfoil - 1; // Convert 1-based to 0-based
        panel_vec[i].u1[0] = 0; panel_vec[i].u1[1] = 0; panel_vec[i].u1[2] = 0;

        panel_vec[i].x2[0] = p.right_edge.coordinates[0];
        panel_vec[i].x2[1] = p.right_edge.coordinates[1];
        panel_vec[i].x2[2] = p.right_edge.coordinates[2];
        panel_vec[i].c2 = p.right_edge.chord;
        panel_vec[i].eps2 = p.right_edge.incidence_deg * DtR;
        panel_vec[i].BC2 = p.right_edge.boundary_condition;
        panel_vec[i].airfoil2 = p.right_edge.airfoil - 1; // Convert 1-based to 0-based
        panel_vec[i].u2[0] = 0; panel_vec[i].u2[1] = 0; panel_vec[i].u2[2] = 0;

        panel_vec[i].u1[0] += info.U[0];
        panel_vec[i].u1[1] += info.U[1];
        panel_vec[i].u1[2] += info.U[2];
        panel_vec[i].u2[0] += info.U[0];
        panel_vec[i].u2[1] += info.U[1];
        panel_vec[i].u2[2] += info.U[2];
    }

    // Trailing edge element indices calculation
    if (info.nopanel > 0) {
        panel_vec[0].TE1 = panel_vec[0].n * (info.m - 1);
        panel_vec[0].TE2 = panel_vec[0].TE1 + panel_vec[0].n - 1;

        for (int i = 1; i < info.nopanel; i++) {
            panel_vec[i].TE1 = panel_vec[i-1].TE2 + 1 + panel_vec[i].n * (info.m - 1);
            panel_vec[i].TE2 = panel_vec[i].TE1 + panel_vec[i].n - 1;
        }
    }

    // 6. Vertical tail
    info.noVT = static_cast<int>(vertical_tail.panels.size());
    if (info.noVT > 5) {
        throw std::runtime_error("Configuration error: maximum number of vertical tail panels is 5");
    }
    for (int i = 0; i < info.noVT; ++i) {
        VTchord[i] = vertical_tail.panels[i].chord;
        VTarea[i] = vertical_tail.panels[i].area;
        VTairfoil[i] = vertical_tail.panels[i].airfoil - 1;
    }

    // 7. Fuselage
    info.noFus = static_cast<int>(fuselage.diameters.size());
    if (info.noFus > 20) {
        throw std::runtime_error("Configuration error: maximum number of fuselage sections is 20");
    }
    delFus = fuselage.section_width;
    FusLT = fuselage.transition_panel > 0 ? fuselage.transition_panel - 1 : 0;
    for (int i = 0; i < info.noFus; ++i) {
        FusSectS[i] = fuselage.diameters[i] * delFus * Pi;
    }

    // 8. Interference drag
    IFdrag = drag.interference_drag_percent * 0.01;
}

std::string Config::getAirfoilFilePath(int airfoil_index_0based) const {
    if (airfoil_index_0based < static_cast<int>(airfoils.files.size())) {
        return airfoils.files[airfoil_index_0based];
    }
    std::string dir = airfoils.directory.empty() ? "airfoils/" : airfoils.directory;
    if (dir.back() != '/' && dir.back() != '\\') {
        dir += '/';
    }
    return dir + "airfoil" + std::to_string(airfoil_index_0based + 1) + ".dat";
}
