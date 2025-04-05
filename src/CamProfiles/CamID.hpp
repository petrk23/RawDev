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

/// <summary>
/// Enumeration of supported cameras
/// </summary>
enum class CamID
{
    // Canon EOS 1D Lineup
    EOS_1DX, EOS_1DX2,

    // Canon EOS 5D Lineup
    EOS_5D, EOS_5D2, EOS_5D3, EOS_5D4,
    EOS_5Ds, EOS_5DsR,

    // Canon EOS 6D Lineup
    EOS_6D, EOS_6D2,

    // Canon EOS 7D Lineup
    EOS_7D, EOS_7D2,

    // Other Canon cameras
    EOS_77D, EOS_80D
};
