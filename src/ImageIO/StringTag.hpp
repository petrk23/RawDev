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
#include <string>

class StringTag : public TiffTag
{
    std::string m_value;

public:
    StringTag(TiffTag::ID id, const std::string& value);

    virtual void write(std::ofstream& file,
        uint32_t& offset, std::vector<char>& extraBytes);
    virtual TiffTag* clone() const;
    virtual size_t extra() const;
    void getVal(std::string& val) {
        val = m_value;
    }
};

////////////////////////////////////////////////////////////////////////////////

inline StringTag::StringTag(TiffTag::ID id, const std::string& value)
    : TiffTag(id, TiffTag::Type::ASCII, 0, 0), m_value(value)
{
    m_tag.val.count = static_cast<uint32_t>(value.length() + 1);
    return;
}
