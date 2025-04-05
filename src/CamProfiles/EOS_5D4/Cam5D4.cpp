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

#include "Cam5D4.hpp"

Cam5D4::Cam5D4(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({512, 512, 512});
    setWhiteLevel({14008, 14008, 14008});
    setBaseExposure(0.13);
    setActiveArea(Rect(Point(136, 42), Point(6880, 4544)));
    setCrop(Rect::Create(Point(148, 54), 6720, 4480));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam5D4::k_forwardMat1 = {{
    { 0.5716, 0.2264, 0.1663 },
    { 0.3791, 0.5665, 0.0544 },
    { 0.2297, 0.0006, 0.5948 }
}};
const Mat3x3 Cam5D4::k_forwardMat2 = {{
    { 0.5497, 0.1714, 0.2433 },
    { 0.3179, 0.6022, 0.0799 },
    { 0.1484, 0.0025, 0.6742 }
}};

const Mat3x3 Cam5D4::k_colorMat1 = {{
    {  0.6515, -0.0927, -0.0634 },
    { -0.4068,  1.1151,  0.3359 },
    { -0.0641,  0.1839,  0.7328 }
}};
const Mat3x3 Cam5D4::k_colorMat2 = {{
    {  0.6446, -0.0366, -0.0864 },
    { -0.4436,  1.2204,  0.2513 },
    { -0.0952,  0.2496,  0.6348 }
}};

// clang-format on
