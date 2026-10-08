#include <Eigen/Dense>
#include <Eigen/LU>
#include <memory>
#include <cstdio>
#include <cmath>

namespace {
    static std::unique_ptr<Eigen::PartialPivLU<Eigen::MatrixXd>> g_lu_solver;
}

//===================================================================//
// START GaussSolve
//===================================================================//
void GaussSolve(double **A, double *R, const int n, double *x)
{
    Eigen::MatrixXd mat(n, n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            mat(i, j) = A[i][j];
        }
    }
    Eigen::Map<const Eigen::VectorXd> rhs(R, n);
    Eigen::Map<Eigen::VectorXd> sol(x, n);

    Eigen::PartialPivLU<Eigen::MatrixXd> lu(mat);
    sol = lu.solve(rhs);
    for (int i = 0; i < n; ++i) {
        if (std::abs(sol[i]) < 1e-15 || sol[i] == 0.0) {
            sol[i] = 0.0;
        }
    }
}
//===================================================================//
// END GaussSolve
//===================================================================//

//===================================================================//
// START LU_Decomposition
//===================================================================//
void LU_Decomposition(double **a, int n, int *indx)
{
    Eigen::MatrixXd mat(n, n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            mat(i, j) = a[i][j];
        }
    }

    g_lu_solver = std::make_unique<Eigen::PartialPivLU<Eigen::MatrixXd>>(mat);

    if (indx) {
        const auto& P = g_lu_solver->permutationP();
        for (int i = 0; i < n; ++i) {
            indx[i] = P.indices()[i];
        }
    }
}
//===================================================================//
// END LU_Decomposition
//===================================================================//

//===================================================================//
// START LU_Solver
//===================================================================//
void LU_Solver(double **a, const int n, const int *indx, double *b)
{
    Eigen::Map<Eigen::VectorXd> rhs(b, n);
    if (g_lu_solver && g_lu_solver->rows() == n) {
        rhs = g_lu_solver->solve(rhs.eval());
    } else {
        Eigen::MatrixXd mat(n, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                mat(i, j) = a[i][j];
            }
        }
        Eigen::PartialPivLU<Eigen::MatrixXd> lu(mat);
        rhs = lu.solve(rhs.eval());
    }

    for (int i = 0; i < n; ++i) {
        if (std::abs(rhs[i]) < 1e-15 || rhs[i] == 0.0) {
            rhs[i] = 0.0;
        }
    }
}
//===================================================================//
// END LU_Solver
//===================================================================//
