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

#include "TiffTag.hpp"

class ByteTag : public TiffTag
{
    std::vector<char> m_values;

public:
    ByteTag(TiffTag::ID id, const char* values, size_t count);
    ByteTag(TiffTag::ID id, const std::vector<char>& values);

public:
    virtual void write(std::ofstream& file,
        uint32_t& offset, std::vector<char>& extraBytes);
    virtual TiffTag* clone() const;
    virtual size_t extra() const;
};
