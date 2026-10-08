#pragma once

#include <Eigen/Dense>
#include <cmath>

inline void Glob_Star(const double x[3], const double nu,
                      const double eps, const double psi, double xsi[3])
{
    // transformation from x-reference frame to xsi_star-reference frame
    // first rotation:  x   -> x', rotation about x-axis by nu
    // second rotation: x'  -> xsi, rotation about y'-axis by eps
    // third rotation:  xsi -> xsi*, rotation about zeta-axis by psi
    // all rotations follow right-hand rule
    const double cnu = std::cos(nu), snu = std::sin(nu);
    const double ceps = std::cos(eps), seps = std::sin(eps);
    const double cpsi = std::cos(psi), spsi = std::sin(psi);

    Eigen::Matrix3d rot;
    rot << cpsi * ceps,  cpsi * seps * snu + spsi * cnu, -cpsi * seps * cnu + spsi * snu,
          -spsi * ceps, -spsi * seps * snu + cpsi * cnu,  spsi * seps * cnu + cpsi * snu,
                  seps,                     -ceps * snu,                       ceps * cnu;

    Eigen::Map<Eigen::Vector3d> out(xsi);
    out = rot * Eigen::Map<const Eigen::Vector3d>(x);
}

inline void Star_Glob(const double xsi[3], const double nu,
                      const double eps, const double psi, double x[3])
{
    // transformation from xsi*-reference frame to x-reference frame
    // third rotation:  xsi* -> xsi, rotation about zeta*-axis by -psi
    // second rotation: xsi  -> x', rotation about eta-axis by -eps
    // first rotation:  x'   -> x, rotation about x'-axis by -nu
    // all rotations follow right-hand rule
    const double cnu = std::cos(nu), snu = std::sin(nu);
    const double ceps = std::cos(eps), seps = std::sin(eps);
    const double cpsi = std::cos(psi), spsi = std::sin(psi);

    Eigen::Matrix3d rot;
    rot << cpsi * ceps,  cpsi * seps * snu + spsi * cnu, -cpsi * seps * cnu + spsi * snu,
          -spsi * ceps, -spsi * seps * snu + cpsi * cnu,  spsi * seps * cnu + cpsi * snu,
                  seps,                     -ceps * snu,                       ceps * cnu;

    Eigen::Map<Eigen::Vector3d> out(x);
    out = rot.transpose() * Eigen::Map<const Eigen::Vector3d>(xsi);
}

