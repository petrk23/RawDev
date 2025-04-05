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

#include "Cam1DX.hpp"

Cam1DX::Cam1DX(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({2048, 2048, 2048});
    setWhiteLevel({15100, 15100, 15100});
    setActiveArea(Rect(Point(126, 100), Point(5344, 3582)));
    setCrop(Rect::Create(Point(144, 114), 5184, 3456));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam1DX::k_forwardMat1 = {{
    { 0.8042, -0.0130,  0.1731 },
    { 0.2588,  0.8230, -0.0818 },
    { 0.0172, -0.4079,  1.2158 }
}};
const Mat3x3 Cam1DX::k_forwardMat2 = {{
    { 0.7653,  0.0735,  0.1255 },
    { 0.2796,  0.8983, -0.1779 },
    { 0.0213, -0.2293,  1.0331 }
}};

const Mat3x3 Cam1DX::k_colorMat1 = {{
    {  0.7151, -0.1143, -0.0790 },
    { -0.3998,  1.1400,  0.2975 },
    { -0.0477,  0.1398,  0.7690 }
}};
const Mat3x3 Cam1DX::k_colorMat2 = {{
    {  0.6847, -0.0614, -0.1014 },
    { -0.4669,  1.2737,  0.2139 },
    { -0.1197,  0.2488,  0.6846 }
}};

// clang-format on
