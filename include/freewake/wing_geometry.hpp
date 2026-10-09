#pragma once

#include "freewake/typedef.hpp"
#include <tuple>
#include <vector>

void Elementary_Wings_Generation(const PANEL panel, const GENERAL info,
                                 BOUND_VORTEX *elementPtr, int &element);

void Trailing_Edge_Generation(const PANEL panel, const GENERAL info,
                              BOUND_VORTEX *elementPtr, BOUND_VORTEX *trailedgePtr,
                              int &element);

std::tuple<std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>>
Identify_Wing_Boundaries(const PANEL* panelPtr, const int nopanel);

void Wing_Generation(const PANEL* panelPtr, const int nopanel,
                     int wing1[5], int wing2[5], int panel1[5], int panel2[5]);

void Surface_DVE_Generation(const GENERAL info, const PANEL* panelPtr,
                            DVE *surfacePtr);

void Move_Flex_Wing(const GENERAL info, DVE* surfacePtr);

void Move_Wing(const GENERAL info, DVE* surfacePtr);
