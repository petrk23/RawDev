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

#include "Cam7D.hpp"

Cam7D::Cam7D(double ctemp)
    : CamProfile(ctemp)
{
    setCFAPattern(CFAPattern::Filter::GBRG);
    setBlackLevel({2047, 2047, 2047});
    setWhiteLevel({13600, 13600, 13600});
    setBaseExposure(-0.21);
    setActiveArea(Rect(Point(158, 51), Point(5360, 3516)));

    // Camera crop
    // CamProfile::m_crop = Rect::CreateRect(168, 56, 5184, 3456);
    // but for clean output it must be tighter...
    setCrop(Rect::Create(Point(168, 58), 5184, 3452));

    setForwardMatrix(k_forwardMat1, k_forwardMat2);
    setColorMatrix(k_colorMat1, k_colorMat2);
    setProfileLook(k_profileLookTableData);
}

// clang-format off

const Mat3x3 Cam7D::k_forwardMat1 = {{
    {  0.5445,  0.3536,  0.0662 },
    {  0.1106,  1.0136, -0.1242 },
    { -0.0374, -0.3559,  1.2184 }
}};
const Mat3x3 Cam7D::k_forwardMat2 = {{
    {  0.7415,  0.1533,  0.0695 },
    {  0.2499,  0.9997, -0.2497 },
    { -0.0022, -0.1933,  1.0207 }
}};

const Mat3x3 Cam7D::k_colorMat1 = {{
    {  1.1620, -0.6350, 0.0005 },
    { -0.2558,  1.0146, 0.2813 },
    {  0.0024,  0.0858, 0.6926 }
}};
const Mat3x3 Cam7D::k_colorMat2 = {{
    {  0.6844, -0.0996, -0.0856 },
    { -0.3876,  1.1761,  0.2396 },
    { -0.0593,  0.1772,  0.6198 }
}};

// clang-format on
