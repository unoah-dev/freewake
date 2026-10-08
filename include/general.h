#pragma once

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <filesystem>
#include <string_view>
#include <algorithm>

#include "typedef.h"
#include "alloc.h"
#include "vector_algebra.h"
#include "ref_frame_transform.h"

inline constexpr double Pi = 3.14159265358979323846;
inline constexpr double DtR = Pi / 180.0;
inline constexpr double RtD = 180.0 / Pi;
inline constexpr double DBL_EPS = 1e-14;
inline const std::filesystem::path OUTPUT_PATH = "output";
inline const std::filesystem::path AIRFOIL_PATH = "airfoils";
inline constexpr const char* PROGRAM_VERSION = "FreeWake2018_Omega";

inline double sanitize_zero(double val, double eps = 1e-15)
{
    if (std::abs(val) < eps || val == 0.0) {
        return 0.0;
    }
    return val;
}

inline std::filesystem::path resolve_output_file(std::string_view filename)
{
    std::filesystem::create_directories(OUTPUT_PATH);
    return OUTPUT_PATH / filename;
}


//global Variables
FILE *test;				//file for test output during debugging


GENERAL info;			//general input information

PANEL *panelPtr;		//pointer holds information on panel geometry

BOUND_VORTEX *elementPtr;//pointer holds information on elementary wings

DVE *surfacePtr;		//pointer to surface Distributed-Vorticity elements
DVE **wakePtr;			//pointer to wake DVE

double Nt_free[2], Nt_ind[2];
						//magnitude of induced and free stream normal
						//forces/density of total wing

double **CN;			//total normal forces, CL,CLi,CY,CYi for each timestep

double *CDi_DVE;		//total ind. drag (Eppler) with DVEs for each timestep
double CDi_finit;		//total induced drag with DVEs after all timesteps

//8-8-07 G.B.
double alpha1,alpha2,alphastep;	//AOA loop, beginning, end, stepsie

