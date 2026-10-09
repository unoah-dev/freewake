#pragma once

#include "freewake/typedef.hpp"

void Delete_timestep();

void Save_Elementary_Wings(const GENERAL info, const BOUND_VORTEX* elementPtr);

void Save_Trailing_Edge(const GENERAL info, const BOUND_VORTEX* trailedgePtr);

void Horstmann_Results(const GENERAL info, const BOUND_VORTEX* elementPtr,
                       const double dt, const double e1, const double e2,
                       const double e3, const double e4, const double CDi_finit,
                       const double e5);

void Header(const GENERAL info, const BOUND_VORTEX* elementPtr,
            const double dt, const double e1, const double e2,
            const double e3, const double e4, const double CDi_finit,
            const double e5);

void Time_Stepping_Results(const GENERAL info, int const first, int const last,
                           double **D, double *R, const double dt,
                           const double e1, const double e2, const double e3,
                           const double e4);

void Time_Stepping_End_Results(const GENERAL info, const int steps, const int nsteps,
                               double **D, double *R, const double dt,
                               const double e1, const double e2, const double e3,
                               const double e4, const double CDi_finit,
                               const double e5);

void Save_Surface_DVEs(const GENERAL info, const DVE *surfacePtr);

void Save_Timestep(const GENERAL info, const int timestep, DVE **wakePtr,
                   const DVE *surfacePtr, double **CN);

void Save_SurfaceDVE_Loads(const GENERAL info, const int timestep,
                           const DVE *surfacePtr);

void Test(const GENERAL info, double **D, const double *R);
