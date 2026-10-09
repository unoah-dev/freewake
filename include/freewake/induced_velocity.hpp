#pragma once

#include "freewake/typedef.hpp"

// Computes induced velocity of system of surface and wake DVEs
void DVE_Induced_Velocity(const GENERAL info, const double P[3], const DVE *surfacePtr,
                          DVE **wakePtr, const int timestep, double w_ind[3]);

// Computes velocity due to surface DVEs
void Surface_DVE_Vel_Induction(const GENERAL info, const double P[3],
                               const DVE *surfacePtr, double w_surface[3]);

// Computes velocity due to wake DVEs
void Wake_DVE_Vel_Induction(const GENERAL info, const double P[3],
                            DVE **wakePtr, const int timestep, double w_wake[3]);

// Computes ind. velocity in a point due to a DVE with possible symmetry
void Single_DVE_Induced_Velocity(const GENERAL info, const DVE dve, const double P[3],
                                 double w_ind[3], const int sym);

// Computes the influence coefficients in P due to a DVE and its possible symmetry
void DVE_Influence_Coeff(const DVE dve, const GENERAL info, const double P[3],
                         double a[3], double b[3], double c[3], const int sym);

// Fixed wake model routines
void Induced_Velocity(const BOUND_VORTEX *elementPtr, const GENERAL info,
                      double const P[3], double w_ind[3]);

void Fixed_Wake_TEinduction(const BOUND_VORTEX *trailedgePtr, const GENERAL info,
                            double const P[3], double w_ind[3]);

void Vortex_Line_Induced_Velocity(const GENERAL info, double const P[3],
                                  double const xo[3], double phi, double nu, double eta,
                                  double A, double B, double C, double w_ind[3]);

void Influence_Coeff(const BOUND_VORTEX vortex, const GENERAL info, double const P[3],
                     double a[3], double b[3], double c[3]);

void BoundVortexInduction(const double P[3], const double xo[3],
                          const double phi, const double nu, const double eta,
                          double a[3], double b[3], double c[3]);

void VortexSheetInduction(const double P[3], const double xo[3],
                          const double phi, const double nu, const double eta,
                          double a[3], double b[3], double c[3], const double singfct);

void FixedWakeInduction(const double P[3], const double xo[3],
                        const double phi, const double nu, const double eta,
                        double a[3], double b[3], double c[3]);
