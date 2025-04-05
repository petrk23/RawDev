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

#include <cinttypes>
#include <cstdlib>

#include "Structures/Mat3x3.hpp"

namespace Color {

struct CIEXYZ {
    double x, y, z;
};

struct CIELab {
    double L, a, b;

    // Distance functions
    [[nodiscard]] double dl(const CIELab& val) const noexcept
    {
        return std::abs(L - val.L);
    }
    [[nodiscard]] double dc(const CIELab& val) const noexcept
    {
        const double adiff = a - val.a, bdiff = b - val.b;
        return adiff * adiff + bdiff * bdiff;
    }
};

struct CIEuv {
    double u, v;
};

struct HSV64 {
    double hue, sat, val;
};
struct RGB64 {
    double r, g, b;
};
struct RGB16 {
    uint16_t r, g, b;
};
struct RGB8 {
    uint8_t r, g, b;
};

////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////

extern const CIEXYZ k_d50;

/* Colorspace conversion matrices */
extern const Mat3x3 k_matXYZtoSRGB;
extern const Mat3x3 k_matXYZtoARGB;
extern const Mat3x3 k_matXYZtoProPhotoRGB;

////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////

void xyz2Lab(const CIEXYZ& in, CIELab& out, const CIEXYZ& white);
Color::HSV64 rgb2hsv(const Color::RGB64& rgbValue);
Color::RGB64 hsv2rgb(const Color::HSV64& hsv);

template<typename TOut, typename TIn>
inline auto xyzTo(const Mat3x3& convMatrix, const TIn& xyz) noexcept
{
    return convMatrix.multiply<TOut>(xyz.x, xyz.y, xyz.z);
}

template<typename TOut, typename TIn>
inline auto rgbTo(const Mat3x3& convMatrix, const TIn& rgb) noexcept
{
    return convMatrix.multiply<TOut>(rgb.r, rgb.g, rgb.b);
}

// Convert uv coordinates to XYZ coordinates
inline CIEXYZ uvToXYZ(const CIEuv& uv, const double Y) noexcept
{
    return {1.5 * uv.u / uv.v, Y, (2.0 - 0.5 * uv.u) / uv.v - 5.0};
}

inline double kelvin2mired(const double temp)
{
    return 1e6 / temp;
}

}; // namespace Color
