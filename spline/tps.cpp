/*
 *  Thin Plate Spline demo/example in C++
 *
 *  - a simple TPS editor, using the Boost uBlas library for large
 *    matrix operations and OpenGL + GLUT for 2D function visualization
 *    (curved plane) and user interface
 *
 *  Copyright (C) 2003,2005 by Jarno Elonen
 *
 *  TPSDemo is Free Software / Open Source with a very permissive
 *  license:
 *
 *  Permission to use, copy, modify, distribute and sell this software
 *  and its documentation for any purpose is hereby granted without fee,
 *  provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear
 *  in supporting documentation.  The authors make no representations
 *  about the suitability of this software for any purpose.
 *  It is provided "as is" without express or implied warranty.
 *
 *  TODO:
 *    - implement TPS approximation 3 as suggested in paper
 *      Gianluca Donato and Serge Belongie, 2002: "Approximation
 *      Methods for Thin Plate Spline Mappings and Principal Warps"
 */

#include <boost/numeric/ublas/matrix.hpp>

#include "linalg3d.h"
#include "ludecomposition.h"

#include <vector>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace boost::numeric::ublas;
using namespace std;

double regularization = 0.0;
double bending_energy = 0.0;

double tps_base_func(double r);
template <typename T>
bool invert(matrix<T> A, matrix<T> &Ainv);
bool abslessthan(double left, double right);
double maxabs(double left, double right);
template <typename T>
matrix<T> multiply(matrix<T> left, matrix<T> right);
matrix<double> tps_transformation(  std::vector<Vec> control_points,
                                    std::vector<Vec> data_points);
std::vector<Vec> loadcontrolpoints(const char *filename);
std::vector<Vec> loaddatapoints(const char *filename);


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: ./tps <control points> <data points>" << endl;
        return -1;
    }

    std::vector<Vec> cp = loadcontrolpoints(argv[1]);
    std::vector<Vec> dp = loaddatapoints(argv[2]);
    cout << dp.size() << endl;
    matrix<double> T = tps_transformation(cp, dp);

    cout << T.size1() << endl;
    cout << T.size2() << endl;

    return 0;
}






// Implementations

double tps_base_func(double r)
{
  if ( r == 0.0 )
    return 0.0;
  else
    return r*r * log(r);
}

template <typename T>
bool invert(matrix<T> A, matrix<T> &Ainv)
{
  unsigned n = A.size1();

  if (n != A.size2())
    return false;

  Ainv = matrix<T>(n,n);
  for (unsigned i = 0; i < n; i++)
  {
    for (unsigned j = 0; j < n; j++)
    {
      Ainv(i,j) = (T) 0;
    }
    Ainv(i,i) = (T) 1;
  }

  return (0 == LU_Solve(A, Ainv));
}

bool abslessthan(double left, double right)
{
    if (left < 0.0)
        left = -left;
    if (right < 0.0)
        right = -right;
    
    if (left < right)
        return true;
    else
        return false;
}

double maxabs(double left, double right)
{
    return abslessthan(left, right) ? right : left;
}

template <typename T>
matrix<T> multiply(matrix<T> left, matrix<T> right)
{
    unsigned nLeftRows = left.size1();
    unsigned nLeftCols = left.size2();
    unsigned nRightRows = right.size1();
    unsigned nRightCols = right.size2();

    // Check the columns in the left matrix against the rows in the right
    if (nLeftCols != nRightRows)
        return matrix<T>(0,0);

    // Initialize matrix to return
    matrix<T> result(nLeftRows, nRightCols);

    // Use the transpose of the right side matrix for spatial locality
    matrix<T> rightTranspose(nRightCols, nRightRows);
    for (unsigned i = 0; i < nRightRows; i++)
    {
        for (unsigned j = 0; j < nRightCols; j++)
        {
            rightTranspose(j,i) = right(i,j);
        }
    }

    double maxelem = 0.0;

    // Perform matrix multiplication
    for (unsigned i = 0; i < nLeftRows; i++)
    {
        for (unsigned j = 0; j < nRightCols; j++)
        {
            T sum = 0;
            for (unsigned k = 0; k < nRightRows; k++)
            {
                sum += left(i,k) * rightTranspose(j,k);
            }
            result(i,j) = sum;
            maxelem = maxabs(maxelem, sum);
        }
    }

    double threshold = maxelem / 1000000.0;

    for (unsigned i = 0; i < nLeftRows; i++)
    {
        for (unsigned j = 0; j < nRightCols; j++)
        {
            if (abslessthan(result(i,j), threshold))
                result(i,j) = 0.0;
        }
    }

    return result;
}

matrix<double> tps_transformation(  std::vector<Vec> control_points,
                                    std::vector<Vec> data_points)
{
    unsigned p = control_points.size();
    unsigned n = data_points.size();

    // Allocate the matrix and vector
    matrix<double> mtx_l(p+4, p+4);
    matrix<double> mtx_orig_k(p, p);

    // Fill K (p x p, upper left of L) and calculate
    // mean edge length from control points
    //
    // K is symmetrical so we really have to
    // calculate only about half of the coefficients.
    double a = 0.0;
    for ( unsigned i=0; i<p; ++i )
    {
        for ( unsigned j=i+1; j<p; ++j )
        {
            Vec pt_i = control_points[i];
            Vec pt_j = control_points[j];
            double elen = (pt_i - pt_j).len();
            mtx_l(i,j) = mtx_l(j,i) =
            mtx_orig_k(i,j) = mtx_orig_k(j,i) = tps_base_func(elen);
            a += elen * 2; // same for upper & lower tri
        }
    }
    a /= (double)(p*p);

    // Fill the rest of L
    for ( unsigned i=0; i<p; ++i )
    {
    // diagonal: regularization parameters (lambda * a^2)
    mtx_l(i,i) = mtx_orig_k(i,i) = regularization * (a*a);

    // P (p x 4, upper right)
    mtx_l(i, p+0) = 1.0;
    mtx_l(i, p+1) = control_points[i].x;
    mtx_l(i, p+2) = control_points[i].y;
    mtx_l(i, p+3) = control_points[i].z;

    // P transposed (4 x p, bottom left)
    mtx_l(p+0, i) = 1.0;
    mtx_l(p+1, i) = control_points[i].x;
    mtx_l(p+2, i) = control_points[i].y;
    mtx_l(p+3, i) = control_points[i].z;
    }

    // O (4 x 4, lower right)
    for ( unsigned i=p; i<p+4; ++i )
        for ( unsigned j=p; j<p+4; ++j )
            mtx_l(i,j) = 0.0;

    // Find L's inverse
    matrix<double> mtx_linv(p+4,p+4);
    if (!invert(mtx_l, mtx_linv))
    {
        puts( "Singular matrix! Aborting." );
        exit(1);
    }

    // Find M
    matrix<double> mtx_m(n,p+4);
    for (unsigned j = 0; j < n; j++)
    {
        for (unsigned i = 0; i < p; i++)
        {
            mtx_m(j,i) = tps_base_func((data_points[j] - control_points[i]).len());
        }

        mtx_m(j,p+0) = 1;
        mtx_m(j,p+1) = data_points[j].x;
        mtx_m(j,p+2) = data_points[j].y;
        mtx_m(j,p+3) = data_points[j].z;
    }

    matrix<double> transform = multiply(mtx_m, mtx_linv);

    return transform;
}

std::vector<Vec> loadcontrolpoints(const char *filename)
{
    ifstream infile(filename);
    string line;
    std::vector<Vec> cp;

    float v0, v1, v2;
    for (int i = 0; getline(infile, line); i++)
    {
        istringstream iss(line);
        iss >> v0 >> v1 >> v2;
        cp.push_back(Vec(v0, v1, v2));
    }

    return cp;
}

std::vector<Vec> loaddatapoints(const char *filename)
{
    ifstream infile(filename);
    string line, label;
    std::vector<Vec> dp;

    float v0, v1, v2;
    for (int i = 0; getline(infile, line); i++)
    {
        istringstream iss(line);
        iss >> label;
        if (label == "v")
        {
            iss >> v0 >> v1 >> v2;
            dp.push_back(Vec(v0, v1, v2));
        }
    }

    return dp;
}

