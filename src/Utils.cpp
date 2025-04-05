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

#include "Utils.hpp"
#include "Color.hpp"
#include "Structures/Mat3x3.hpp"

#include <cassert>

int Utils::compareDouble(const void* a, const void* b)
{
    if (*(double*)a < *(double*)b)
        return -1;
    if (*(double*)a == *(double*)b)
        return 0;
    return 1;
}

Mat3x3 Utils::interpolateMatrices(
    double temp,
    const Mat3x3& mat1, double illu1,
    const Mat3x3& mat2, double illu2)
{
    Mat3x3 res;

    // Outside range
    if (temp >= illu2) {
        return mat2;
    }
    if (temp <= illu1) {
        return mat1;
    }

    // Interpolate between matrices
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            res.mdata[i][j] = Utils::linearInter(
                temp, illu1, mat1.mdata[i][j], illu2, mat2.mdata[i][j]);
        }
    }
    return res;
}

// Inverted(mired) linear interpolation
Mat3x3 Utils::interColorMat(
    double temp,
    const Mat3x3& mat1, double illu1,
    const Mat3x3& mat2, double illu2)
{
    temp = Color::kelvin2mired(temp);
    const double illu1d = Color::kelvin2mired(illu1);
    const double illu2d = Color::kelvin2mired(illu2);
    Mat3x3 colorMat;

    if (illu1d > illu2d) {
        colorMat = interpolateMatrices(
            temp, mat2, illu2d, mat1, illu1d);
    }
    else if (illu1d < illu2d) {
        colorMat = interpolateMatrices(
            temp, mat1, illu1d, mat2, illu2d);
    }
    else {
        assert(illu1d == illu2d);
        colorMat = mat1; // Only one exists
    }
    return colorMat;
}
