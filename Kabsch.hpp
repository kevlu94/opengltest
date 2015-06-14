#ifndef KABSCH_HPP
#define KABSCH_HPP

#include <Eigen/Geometry>

// This code is released in public domain

#include "Kabsch.hpp"

// Given two sets of 3D points, find the rotation + translation + scale
// which best maps the first set to the second.
// Source: http://en.wikipedia.org/wiki/Kabsch_algorithm

// The input 3D points are stored as columns.
Eigen::Affine3d Find3DAffineTransform(Eigen::Matrix3Xd in, Eigen::Matrix3Xd out);

// A function to test Find3DAffineTransform()
void TestFind3DAffineTransform();

#endif