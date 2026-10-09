#pragma once

#include "freewake/typedef.hpp"
#include <cstdio>

void LongitudinalTrim(GENERAL info, PANEL *panelPtr, DVE *surfaceDVEPtr, int HTpanel,
                      double *&cn, double &CL, double &CDi, FILE *MomSol);
