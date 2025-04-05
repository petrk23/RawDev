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

#include "Cam5D3.hpp"

Cam5D3::Cam5D3(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({2047, 2047, 2047});
    setWhiteLevel({15000, 15000, 15000});
    setBaseExposure(0.25);
    setActiveArea(Rect(Point(122, 80), Point(5918, 3950)));
    setCrop(Rect::Create(Point(140, 96), 5760, 3840));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam5D3::k_forwardMat1 = {{
    { 0.7868,  0.0092,  0.1683 },
    { 0.2291,  0.8615, -0.0906 },
    { 0.0027, -0.4752,  1.2976 }
}};
const Mat3x3 Cam5D3::k_forwardMat2 = {{
    { 0.7637,  0.0805,  0.1201 },
    { 0.2649,  0.9179, -0.1828 },
    { 0.0137, -0.2456,  1.0570 }
}};

// Color matrix
const Mat3x3 Cam5D3::k_colorMat1 = {{
    {  0.7234, -0.1413, -0.0600 },
    { -0.3631,  1.1150,  0.2850 },
    { -0.0382,  0.1335,  0.6437 }
}};
const Mat3x3 Cam5D3::k_colorMat2 = {{
    {  0.6722, -0.0635, -0.0963 },
    { -0.4287,  1.2460,  0.2028 },
    { -0.0908,  0.2162,  0.5668 }
}};

// clang-format on
