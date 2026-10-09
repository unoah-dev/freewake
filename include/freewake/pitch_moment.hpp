#pragma once

#include "freewake/typedef.hpp"

double PitchingMoment(const GENERAL info, PANEL *panelPtr, DVE *&surfacePtr,
                      const double cmac, const double epsilonHT,
                      const int HTpanel, const double xCG[3],
                      double &CLht, double &CLhti,
                      double **&N_force, double *&D_force,
                      double &CL, double &CDi_finit);
