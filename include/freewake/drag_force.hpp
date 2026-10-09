#pragma once

#include "freewake/typedef.hpp"

double Induced_DVE_Drag(const GENERAL info, const PANEL* panelPtr,
                        DVE* surfacePtr, DVE** wakePtr,
                        const int rightnow, double* D_force);

double SectionDrag(double profiledata[][5], double Re, double &cl, int rows,
                   double &cm, const int section);
