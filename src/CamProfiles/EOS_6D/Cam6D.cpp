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

#include "Cam6D.hpp"

Cam6D::Cam6D(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({2047, 2047, 2047});
    setWhiteLevel({15000, 15000, 15000});
    setBaseExposure(0.25);
    setActiveArea(Rect(Point(72, 38), Point(5568, 3708)));
    setCrop(Rect::Create(Point(84, 50), 5472, 3648));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam6D::k_forwardMat1 = {{
    {  0.7763,  0.0065,  0.1815 },
    {  0.2364,  0.8351, -0.0715 },
    { -0.0059, -0.4228,  1.2538 }
}};
const Mat3x3 Cam6D::k_forwardMat2 = {{
    { 0.7464,  0.1044,  0.1135 },
    { 0.2648,  0.9173, -0.1820 },
    { 0.0113, -0.2154,  1.0292 }
}};

const Mat3x3 Cam6D::k_colorMat1 = {{
    {  0.7546, -0.1435, -0.0929 },
    { -0.3846,  1.1488,  0.2692 },
    { -0.0332,  0.1209,  0.6370 }
}};
const Mat3x3 Cam6D::k_colorMat2 = {{
    {  0.7034, -0.0804, -0.1014 },
    { -0.4420,  1.2564,  0.2058 },
    { -0.0851,  0.1994,  0.5758 }
}};

// clang-format on
