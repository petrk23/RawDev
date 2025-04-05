/*
 * This file is part of RawDev;
 * see <https://github.com/petrk23/RawDev>.
 *
 * Copyright (C) 2020-2025 Petr Krajník
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Mat3x3.hpp"
#include "Color.hpp"

#include <cassert>
#include <cmath>

/// <summary>
/// Unit matrix constant
/// </summary>
const Mat3x3 Mat3x3::k_unitMatrix = {
    {{1, 0, 0},
     {0, 1, 0},
     {0, 0, 1}}
};

/// <summary>
/// Matrix multiplication
/// </summary>
/// <param name="mat">Multiplication matrix</param>
/// <returns>New multiplied matrix</returns>
Mat3x3 Mat3x3::multiply(const Mat3x3& mat) const noexcept
{
    Mat3x3 res;

    // This simple algorithm is sufficient,
    // as we have matrices of fixed size 3x3.

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                res.mdata[i][j] += mdata[i][k] * mat.mdata[k][j];
            }
        }
    }
    return res;
}

/// <summary>
/// Matrix division by scalar
/// </summary>
/// <param name="val">Value to divide matrix values</param>
void Mat3x3::operator/=(const double val) noexcept
{
    assert(val != 0.0); // Division by zero

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mdata[i][j] /= val;
        }
    }
}

/// <summary>
/// Calculate matrix determinant
/// </summary>
/// <returns>Determinant value</returns>
double Mat3x3::det() const noexcept
{
    // Sarrus rule for 3x3
    return (mdata[0][0] * mdata[1][1] * mdata[2][2]) +
           (mdata[0][1] * mdata[1][2] * mdata[2][0]) +
           (mdata[0][2] * mdata[1][0] * mdata[2][1]) -
           (mdata[2][0] * mdata[1][1] * mdata[0][2]) -
           (mdata[2][1] * mdata[1][2] * mdata[0][0]) -
           (mdata[2][2] * mdata[1][0] * mdata[0][1]);
}

/// <summary>
/// Calculate inverse matrix
/// </summary>
/// <returns>Inverse matrix if exists. If not zero matrix is returned.</returns>
Mat3x3 Mat3x3::inverse() const noexcept
{
    Mat3x3 res; // Zero by construction

    if (isDiagonal()) {
        for (int i = 0; i < 3; ++i) {
            res.mdata[i][i] = 1 / mdata[i][i];
        }
    }
    else if (isInversible()) {
        assert(!isDiagonal());
        assert(det() != 0.0); // Is inversible

        // Cramers rule matrix inversion
        res.mdata[0][0] = mdata[1][1] * mdata[2][2] - mdata[2][1] * mdata[1][2];
        res.mdata[0][1] = mdata[2][1] * mdata[0][2] - mdata[0][1] * mdata[2][2];
        res.mdata[0][2] = mdata[0][1] * mdata[1][2] - mdata[1][1] * mdata[0][2];

        res.mdata[1][0] = mdata[2][0] * mdata[1][2] - mdata[1][0] * mdata[2][2];
        res.mdata[1][1] = mdata[0][0] * mdata[2][2] - mdata[2][0] * mdata[0][2];
        res.mdata[1][2] = mdata[1][0] * mdata[0][2] - mdata[0][0] * mdata[1][2];

        res.mdata[2][0] = mdata[1][0] * mdata[2][1] - mdata[2][0] * mdata[1][1];
        res.mdata[2][1] = mdata[2][0] * mdata[0][1] - mdata[0][0] * mdata[2][1];
        res.mdata[2][2] = mdata[0][0] * mdata[1][1] - mdata[1][0] * mdata[0][1];

        res /= det();
    }
    return res;
}

/// <summary>
/// Check if matrix is diagonal
/// </summary>
/// <returns>True if diagonal. Otherwise false.</returns>
bool Mat3x3::isDiagonal() const noexcept
{
    for (int i = 0; i < 3; ++i) {
        if (mdata[i][i] == 0) {
            return false;
        }
        for (int j = 0; j < 3; ++j) {
            if (i != j && mdata[i][j] != 0) {
                return false;
            }
        }
    }
    return true;
}

/// <summary>
/// Equality test of matrices
/// </summary>
/// <param name="matA">Matrix A</param>
/// <param name="matB">Matrix B</param>
/// <returns>True if matrices are same. Otherwise false.</returns>
bool operator==(const Mat3x3& matA, const Mat3x3& matB) noexcept
{
    constexpr double tolerance = 1e-10; // Precission limits

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (fabs(matA.mdata[i][j] - matB.mdata[i][j]) >= tolerance) {
                return false;
            }
        }
    }
    return true;
}
