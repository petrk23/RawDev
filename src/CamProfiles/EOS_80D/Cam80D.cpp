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

#include "Cam80D.hpp"

Cam80D::Cam80D(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({511, 511, 511});
    setWhiteLevel({11892, 11892, 11892});
    setActiveArea(Rect(Point(264, 34), Point(6288, 4056)));
    setCrop(Rect::Create(Point(276, 46), 6000, 4000));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam80D::k_forwardMat1 = {{
    { 0.5369, 0.2694, 0.1580 },
    { 0.3315, 0.6187, 0.0498 },
    { 0.1953, 0.0003, 0.6295 }
}};
const Mat3x3 Cam80D::k_forwardMat2 = {{
    { 0.5443, 0.1924, 0.2276 },
    { 0.3270, 0.5988, 0.0743 },
    { 0.1701, 0.0008, 0.6542 }
}};

const Mat3x3 Cam80D::k_colorMat1 = {{
    {  0.8320, -0.1998, -0.0078 },
    { -0.3476,  1.0481,  0.3480 },
    { -0.0384,  0.1349,  0.5901 }
}};
const Mat3x3 Cam80D::k_colorMat2 = {{
    {  0.7457, -0.0671, -0.0937 },
    { -0.4849,  1.2495,  0.2643 },
    { -0.1213,  0.2354,  0.5492 }
}};

// clang-format on
