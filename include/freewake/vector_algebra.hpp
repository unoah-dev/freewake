#pragma once

#include <Eigen/Dense>
#include <cmath>

inline double norm2(const double v[3])
{
    return Eigen::Map<const Eigen::Vector3d>(v).norm();
}

inline double dot(const double v1[3], const double v2[3])
{
    return Eigen::Map<const Eigen::Vector3d>(v1).dot(Eigen::Map<const Eigen::Vector3d>(v2));
}

inline void cross(const double v1[3], const double v2[3], double result[3])
{
    Eigen::Map<Eigen::Vector3d> r(result);
    r = Eigen::Map<const Eigen::Vector3d>(v1).cross(Eigen::Map<const Eigen::Vector3d>(v2));
}

inline void vsum(const double v1[3], const double v2[3], double result[3])
{
    Eigen::Map<Eigen::Vector3d> r(result);
    r = Eigen::Map<const Eigen::Vector3d>(v1) + Eigen::Map<const Eigen::Vector3d>(v2);
}

inline void scalar(const double v1[3], const double v2, double result[3])
{
    Eigen::Map<Eigen::Vector3d> r(result);
    r = Eigen::Map<const Eigen::Vector3d>(v1) * v2;
}

inline void rotateX(const double v1[3], const double alpha, double result[3])
{
    // transforms vector v1 in new coordinate system rotated by alpha around x-axis (RHS)
    const double c = std::cos(alpha);
    const double s = std::sin(alpha);
    Eigen::Matrix3d R;
    R << 1.0, 0.0, 0.0,
         0.0,   c,   s,
         0.0,  -s,   c;
    Eigen::Map<Eigen::Vector3d> r(result);
    r = R * Eigen::Map<const Eigen::Vector3d>(v1);
}

