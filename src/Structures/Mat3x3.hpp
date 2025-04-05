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

#pragma once

struct Mat3x3 {
    static const Mat3x3 k_unitMatrix;
    double mdata[3][3] = {}; // Zeroed init matrix data

    template<typename T>
    [[nodiscard]] T multiply(double a, double b, double c) const noexcept;
    [[nodiscard]] Mat3x3 multiply(const Mat3x3& mat) const noexcept;
    [[nodiscard]] Mat3x3 operator*(const Mat3x3& mat) const noexcept;

    void operator/=(double val) noexcept;

    [[nodiscard]] double det() const noexcept;
    [[nodiscard]] Mat3x3 inverse() const noexcept;
    [[nodiscard]] bool isInversible() const noexcept;
    [[nodiscard]] bool isDiagonal() const noexcept;
};

bool operator==(const Mat3x3& matA, const Mat3x3& matB) noexcept;

///////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Multiply matrix by vector
/// </summary>
/// <typeparam name="T">Resulting vector type</typeparam>
/// <param name="a">First vector value</param>
/// <param name="b">Second vector value</param>
/// <param name="c">Third vector value</param>
/// <returns>New multiplied vector</returns>
template<typename T> inline T Mat3x3::multiply(
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    const double a, const double b, const double c) const noexcept
{
    return {
        mdata[0][0] * a + mdata[0][1] * b + mdata[0][2] * c,
        mdata[1][0] * a + mdata[1][1] * b + mdata[1][2] * c,
        mdata[2][0] * a + mdata[2][1] * b + mdata[2][2] * c
    };
}

inline Mat3x3 Mat3x3::operator*(const Mat3x3& mat) const noexcept
{
    return multiply(mat);
}

inline bool operator!=(const Mat3x3& matA, const Mat3x3& matB) noexcept
{
    return !(matA == matB);
}

inline bool Mat3x3::isInversible() const noexcept
{
    return det() != 0.0;
}
