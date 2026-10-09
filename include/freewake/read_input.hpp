#pragma once

#include "freewake/typedef.hpp"

void General_Info_from_File(GENERAL &info, double &alpha1, double &alpha2, double &alphastep);

void Panel_Info_from_File(PANEL *panelPtr, const GENERAL info);

void VT_Fus_Info(GENERAL &info, double VTchord[5], double VTarea[5], int VTairfoil[5],
                 double FusSectS[20], double &delFus, int &FusLT, double &IFdrag);

void Read_Timestep(int timestep, DVE *surfaceDVE, DVE **wakeDVE);
