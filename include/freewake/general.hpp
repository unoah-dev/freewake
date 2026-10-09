#pragma once

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <filesystem>
#include <string_view>
#include <algorithm>

#include "freewake/typedef.hpp"
#include "freewake/alloc.hpp"
#include "freewake/vector_algebra.hpp"
#include "freewake/ref_frame_transform.hpp"

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
inline FILE *test = nullptr;				//file for test output during debugging


inline GENERAL info{};			//general input information

inline PANEL *panelPtr = nullptr;		//pointer holds information on panel geometry

inline BOUND_VORTEX *elementPtr = nullptr;//pointer holds information on elementary wings

inline DVE *surfacePtr = nullptr;		//pointer to surface Distributed-Vorticity elements
inline DVE **wakePtr = nullptr;			//pointer to wake DVE

inline double Nt_free[2] = {0.0, 0.0};
inline double Nt_ind[2] = {0.0, 0.0};
						//magnitude of induced and free stream normal
						//forces/density of total wing

inline double **CN = nullptr;			//total normal forces, CL,CLi,CY,CYi for each timestep

inline double *CDi_DVE = nullptr;		//total ind. drag (Eppler) with DVEs for each timestep
inline double CDi_finit = 0.0;		//total induced drag with DVEs after all timesteps

//8-8-07 G.B.
inline double alpha1 = 0.0, alpha2 = 0.0, alphastep = 0.0;	//AOA loop, beginning, end, stepsie

