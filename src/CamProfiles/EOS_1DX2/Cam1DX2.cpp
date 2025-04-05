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

#include "Cam1DX2.hpp"

Cam1DX2::Cam1DX2(double ctemp)
    : CamProfile(ctemp)
{
    setBlackLevel({512, 512, 512});
    setWhiteLevel({13898, 13898, 13898});
    setBaseExposure(0.3);
    setActiveArea(Rect(Point(72, 38), Point(5568, 3708)));
    setCrop(Rect::Create(Point(84, 50), 5472, 3648));
    CAMPROFILE_INIT_STRUCTURES;
}

// clang-format off

const Mat3x3 Cam1DX2::k_forwardMat1 = {{
    { 0.5414, 0.2703, 0.1526 },
    { 0.3260, 0.6274, 0.0467 },
    { 0.1967, 0.0001, 0.6283 }
}};
const Mat3x3 Cam1DX2::k_forwardMat2 = {{
    { 0.5282, 0.2182, 0.2179 },
    { 0.3133, 0.6156, 0.0711 },
    { 0.1684, 0.0018, 0.6549 }
}};

const Mat3x3 Cam1DX2::k_colorMat1 = {{
    {  0.8101, -0.2023, -0.0221},
    { -0.3652,  1.0819,  0.3276},
    { -0.0491,  0.1461,  0.6583}
}};
const Mat3x3 Cam1DX2::k_colorMat2 = {{
    {  0.7596, -0.0978, -0.0967},
    { -0.4808,  1.2571,  0.2503},
    { -0.1398,  0.2567,  0.5752}
}};

// clang-format on
