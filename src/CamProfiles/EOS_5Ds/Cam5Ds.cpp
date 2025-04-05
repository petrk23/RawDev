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

#include "Cam5Ds.hpp"

Cam5Ds::Cam5Ds(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({2047, 2047, 2047});
    setWhiteLevel({14466, 14466, 14466});
    setBaseExposure(0.17);
    setActiveArea(Rect(Point(160, 64), Point(8896, 5920)));
    setCrop(Rect::Create(Point(196, 100), 8688, 5792));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam5Ds::k_forwardMat1 = {{
    { 0.5518, 0.2528, 0.1597 },
    { 0.3380, 0.6092, 0.0528 },
    { 0.2019, 0.0004, 0.6229 }
}};
const Mat3x3 Cam5Ds::k_forwardMat2 = {{
    { 0.5470, 0.1929, 0.2244 },
    { 0.3283, 0.5982, 0.0734 },
    { 0.1679, 0.0004, 0.6567 }
}};

const Mat3x3 Cam5Ds::k_colorMat1 = {{
    {  0.6848, -0.1661, -0.0221 },
    { -0.3904,  1.0931,  0.3434 },
    { -0.0470,  0.1251,  0.6039 }
}};
const Mat3x3 Cam5Ds::k_colorMat2 = {{
    {  0.6250, -0.0711, -0.0808 },
    { -0.5153,  1.2794,  0.2636 },
    { -0.1249,  0.2198,  0.5610 }
}};

// clang-format off
