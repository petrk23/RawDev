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

#include "Cam6D2.hpp"

Cam6D2::Cam6D2(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({513, 513, 513});
    setWhiteLevel({14558, 14558, 14558});
    setBaseExposure(0.25);
    setActiveArea(Rect(Point(120, 44), Point(6384, 4224)));
    setCrop(Rect::Create(Point(132, 56), 6240, 4160));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam6D2::k_forwardMat1 = {{
    { 0.5806, 0.2293, 0.1544 },
    { 0.3687, 0.5807, 0.0505 },
    { 0.2066, 0.0010, 0.6176 }
}};
const Mat3x3 Cam6D2::k_forwardMat2 = {{
    { 0.5333, 0.1999, 0.2311 },
    { 0.3072, 0.6184, 0.0744 },
    { 0.1433, 0.0024, 0.6794 }
}};

const Mat3x3 Cam6D2::k_colorMat1 = {{
    {  0.7295, -0.1664, -0.0669 },
    { -0.4871,  1.2238,  0.2980 },
    { -0.0585,  0.1356,  0.6696 }
}};
const Mat3x3 Cam6D2::k_colorMat2 = {{
    {  0.6875, -0.0970, -0.0932 },
    { -0.4691,  1.2459,  0.2501 },
    { -0.0874,  0.1953,  0.5809 }
}};

// clang-format on
