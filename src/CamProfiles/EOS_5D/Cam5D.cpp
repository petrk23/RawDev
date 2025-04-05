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

#include "Cam5D.hpp"

Cam5D::Cam5D(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({128, 128, 128});
    setWhiteLevel({3692, 3692, 3692});
    setBaseExposure(0.0); // Ignoring the 0.25 to match neutral grey
    setActiveArea(Rect(Point(90, 34), Point(4476, 2954)));

    // Camera crop
    // CamProfile::m_crop = Rect::CreateRect(100, 39, 4368, 2912);
    // but for clean output it must be tighter...
    setCrop(Rect::Create(Point(100, 40), 4368, 2908));

    setForwardMatrix(k_forwardMat1, k_forwardMat2);
    setColorMatrix(k_colorMat1, k_colorMat2);
}

// clang-format off

const Mat3x3 Cam5D::k_forwardMat1 = {{
    { 0.8141,  0.0349,  0.1153 },
    { 0.3909,  0.6662, -0.0571 },
    { 0.1095, -0.2533,  0.9689 }
}};
const Mat3x3 Cam5D::k_forwardMat2 = {{
    { 0.7903,  0.0519,  0.1221 },
    { 0.3935,  0.7150, -0.1085 },
    { 0.0650, -0.1350,  0.8952 }
}};

const Mat3x3 Cam5D::k_colorMat1 = {{
    {  0.7284, -0.1569, -0.0425 },
    { -0.6726,  1.4015,  0.2993 },
    { -0.0926,  0.1258,  0.7774 }
}};
const Mat3x3 Cam5D::k_colorMat2 = {{
    {  0.6347, -0.0479, -0.0972 },
    { -0.8297,  1.5954,  0.2480 },
    { -0.1968,  0.2131,  0.7649 }
}};

// clang-format on
