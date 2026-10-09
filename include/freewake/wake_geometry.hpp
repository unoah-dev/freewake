#pragma once

#include "freewake/typedef.hpp"

void Squirt_out_Wake(const GENERAL info, const PANEL *panelPtr,
                     DVE *surfacePtr, DVE *wakePtr);

void Relax_Wake(const GENERAL info, const int rightnow,
                const DVE *surfacePtr, DVE **wakePtr);

void Edge_Point(const double xo[3], const double nu, const double epsilon,
                const double psi, const double eta, const double xsi,
                const double phi, double x[3]);

void Displace_Point(const GENERAL info, const int rightnow,
                    const double x[3], const double u[3], double x_new[3]);

void New_xo(const GENERAL info, DVE &wakeDVE, double x_left[3], double x_right[3]);

void New_eta_nu_eps_psi_xsi(DVE &wakeDVE, const DVE US_DVE);

void New_wakeDVE0(const GENERAL info, DVE *wakeDVE0, const DVE *wakeDVE1);

void New_vorticity_coefficients(const GENERAL info, const PANEL *panePtr,
                                DVE *wakeDVE, const DVE *surfacePtr);
