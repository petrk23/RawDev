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

#pragma once

// Macros that reduce typing and managing cam
// profiles class hiearchy.

#define CAMPROFILE_BASICS(cam, name)                    \
    static constexpr std::string_view k_camName = name; \
    explicit Cam##cam(double ctemp);                    \
    std::string_view getCameraName() const override     \
    {                                                   \
        return k_camName;                               \
    }                                                   \
    CamID getCameraID() const override                  \
    {                                                   \
        return CamID::EOS_##cam;                        \
    }

#define CAMPROFILE_INIT_STRUCTURES                  \
    setForwardMatrix(k_forwardMat1, k_forwardMat2); \
    setColorMatrix(k_colorMat1, k_colorMat2);       \
    setHSVMaps(k_hsvMapData1, k_hsvMapData2);       \
    setProfileLook(k_profileLookTableData)
