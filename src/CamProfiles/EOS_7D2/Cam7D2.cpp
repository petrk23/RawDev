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

#include "Cam7D2.hpp"

Cam7D2::Cam7D2(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({2047, 2047, 2047});
    setWhiteLevel({13400, 13400, 13400});
    setBaseExposure(0.25);
    setActiveArea(Rect(Point(72, 38), Point(5568, 3708)));
    setCrop(Rect::Create(Point(84, 50), 5472, 3648));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam7D2::k_forwardMat1 = {{
    { 0.5165, 0.2766, 0.1712 },
    { 0.2991, 0.6466, 0.0544 },
    { 0.1694, 0.0011, 0.6546 }
}};
const Mat3x3 Cam7D2::k_forwardMat2 = {{
    { 0.5160, 0.2078, 0.2405 },
    { 0.3024, 0.6191, 0.0786 },
    { 0.1525, 0.0026, 0.6700 }
}};

const Mat3x3 Cam7D2::k_colorMat1 = {{
    {  0.7795, -0.2196, -0.0251 },
    { -0.3391,  1.0273,  0.3632 },
    { -0.0373,  0.1283,  0.6613 }
}};
const Mat3x3 Cam7D2::k_colorMat2 = {{
    {  0.7268, -0.1082, -0.0969 },
    { -0.4186,  1.1839,  0.2663 },
    { -0.0825,  0.2029,  0.5839 }
}};

// clang-format on
