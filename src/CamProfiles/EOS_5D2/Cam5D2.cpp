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

#include "Cam5D2.hpp"

Cam5D2::Cam5D2(double ctemp)
    : CamProfile(ctemp)
{
    setCFAPattern(CFAPattern::Filter::GBRG);
    setBlackLevel({1024, 1024, 1024});
    setWhiteLevel({15600, 15600, 15600});
    setBaseExposure(0.4);
    setActiveArea(Rect(Point(158, 51), Point(5792, 3804)));

    // Camera crop
    // CamProfile::m_crop = Rect::CreateRect(168, 56, 5616, 3744);
    // but for clean output it must be tighter...
    setCrop(Rect::Create(Point(168, 57), 5616, 3742));

    setForwardMatrix(k_forwardMat);
    setColorMatrix(k_colorMat1, k_colorMat2);
    setProfileLook(k_profileLookTableData);
}

// clang-format off

const Mat3x3 Cam5D2::k_forwardMat = {{
    { 0.8924, -0.1041,  0.1760 },
    { 0.4351,  0.6621, -0.0972 },
    { 0.0505, -0.1562,  0.9308 }
}};

const Mat3x3 Cam5D2::k_colorMat1 = {{
    {  0.5309, -0.0229, -0.0336 },
    { -0.6241,  1.3265,  0.3337 },
    { -0.0817,  0.1215,  0.6664 }
}};
const Mat3x3 Cam5D2::k_colorMat2 = {{
    {  0.4716, 0.0603, -0.0830 },
    { -0.7798, 1.5474,  0.2480 },
    { -0.1496, 0.1937,  0.6651 }
}};

// clang-format on
