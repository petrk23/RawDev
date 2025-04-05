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

#include "Cam77D.hpp"

Cam77D::Cam77D(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({511, 511, 511});
    setWhiteLevel({11892, 11892, 11892});
    setActiveArea(Rect(Point(264, 34), Point(6288, 4056)));
    setCrop(Rect::Create(Point(276, 48), 6000, 4000));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam77D::k_forwardMat1 = {{
    { 0.5407, 0.2506, 0.1730 },
    { 0.3306, 0.6136, 0.0558 },
    { 0.1852, 0.0007, 0.6392 }
}};
const Mat3x3 Cam77D::k_forwardMat2 = {{
    { 0.5388, 0.1799, 0.2457 },
    { 0.3091, 0.6107, 0.0802 },
    { 0.1438, 0.0001, 0.6812 }
}};

const Mat3x3 Cam77D::k_colorMat1 = {{
    {  0.7952, -0.1689, -0.0575 },
    { -0.3746,  1.0825,  0.3378 },
    { -0.0405,  0.1362,  0.6120 }
}};
const Mat3x3 Cam77D::k_colorMat2 = {{
    {  0.7377, -0.0742, -0.0998 },
    { -0.4235,  1.1981,  0.2549 },
    { -0.0673,  0.1918,  0.5538 }
}};

// clang-format on
