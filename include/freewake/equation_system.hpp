#pragma once

#include "freewake/typedef.hpp"

void FlexWingVortDist(const GENERAL info, const PANEL *panelPtr,
                      DVE *surfacePtr, DVE **wakePtr, double **D,
                      double *R, int timestep);

void DVE_Vorticity_Distribution(const GENERAL info, const PANEL *panelPtr,
                                DVE *surfacePtr, DVE **wakePtr,
                                const int timestep, double **D, double *R,
                                const int *pivot);

void DVE_Resultant(const GENERAL info, const PANEL *panelPtr,
                   const DVE *surfacePtr, DVE **wakePtr, const int timestep,
                   double *R);

void DVE_Resultant(const GENERAL info, const DVE *surfacePtr,
                   DVE **wakePtr, const int timestep,
                   double *R);

void Vorticity_Distribution(const GENERAL info, const PANEL *panelPtr,
                            BOUND_VORTEX *elementPtr,
                            double **A, double *R);

void Resultant(const BOUND_VORTEX *elementPtr, const GENERAL info, double *R);

void BoundaryCond(const BOUND_VORTEX *elementPtr, const PANEL *panelPtr,
                  const GENERAL info, double **A);

void DVE_KinCond(DVE *surfacePtr, const GENERAL info,
                 const PANEL *panelPtr, double **D);

void KinematicCond(const BOUND_VORTEX *elementPtr, const GENERAL info,
                   const PANEL *panelPtr, double **A);

void Trailing_Edge_Vorticity(const GENERAL info, const PANEL *panelPtr,
                             BOUND_VORTEX *elementPtr,
                             BOUND_VORTEX *trailedgePtr);
