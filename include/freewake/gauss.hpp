#pragma once

#include <Eigen/Dense>
#include <optional>

std::optional<Eigen::VectorXd> SolveLinearSystem(double **A, double *R, const int n);
void GaussSolve(double **A, double *R, const int n, double *x);
void LU_Decomposition(double **a, int n, int *indx);
void LU_Solver(double **a, const int n, const int *indx, double *b);
