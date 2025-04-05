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

#include <cinttypes>

struct TiffHeader
{
    char byteOrder[2]; // Endianess
    uint16_t magic; // Tiff magic number 42
    uint32_t firstIFDoffset; // Offset of the first IFD

    TiffHeader() : magic(42), firstIFDoffset(0)
    {
        const uint32_t numb = 1;
        const char* const p = reinterpret_cast<const char*>(&numb);

        // Endianess setup
        if (p[0] == 1) {// Little Endian
            byteOrder[0] = 'I'; byteOrder[1] = 'I';
        }
        else {// Big Endian
            byteOrder[0] = 'M'; byteOrder[1] = 'M';
        }
        return;
    }
};
