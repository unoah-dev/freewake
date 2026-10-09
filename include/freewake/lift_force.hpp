#pragma once

#include "freewake/typedef.hpp"
#include <tuple>
#include <array>

std::tuple<std::array<double, 2>, std::array<double, 2>, double, double, double, double>
Compute_Wing_Normal_Forces(const GENERAL& info, double **N_force);

void DVE_Wing_Normal_Forces(const GENERAL info, double **N_force,
                            double Nt_free[2], double Nt_ind[2],
                            double &CL, double &CLi, double &CY, double &CYi);

void Surface_DVE_Normal_Forces(const GENERAL info, const PANEL *panelPtr,
                               const int timestep, DVE **wakePtr,
                               DVE *surfacePtr, double **N_force);

void Elementary_Wing_Normal_Forces(const PANEL* panelPtr, const GENERAL info,
                                   const BOUND_VORTEX* elementPtr, BOUND_VORTEX* trailedgePtr);

void Wing_Normal_Forces(const PANEL* panelPtr, const GENERAL info,
                        const BOUND_VORTEX* elementPtr, BOUND_VORTEX* trailedgePtr,
                        double Nt_free[2], double Nt_ind[2],
                        double &CL, double &CLi, double &CY, double &CYi);
