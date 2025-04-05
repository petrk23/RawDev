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

class ShortTag : public TiffTag
{
    std::vector<uint16_t> m_values;

public:
    ShortTag(TiffTag::ID id, uint16_t val);
    ShortTag(TiffTag::ID id, uint16_t val1, uint16_t val2, uint16_t val3);
    ShortTag(TiffTag::ID id, const std::vector<uint16_t>& values);

public:
    virtual void write(std::ofstream& file,
        uint32_t& offset, std::vector<char>& extraBytes);
    virtual TiffTag* clone() const;
    virtual size_t extra() const;
    void getVal(std::vector<uint16_t>& val) const {
        val = m_values;
    }
};

////////////////////////////////////////////////////////////////////////////////

inline ShortTag::ShortTag(TiffTag::ID id, uint16_t val)
    : TiffTag(id, TiffTag::Type::SHORT, 1, 0), m_values()
{
    m_values.push_back(val);
    return;
}

inline ShortTag::ShortTag(
    TiffTag::ID id, uint16_t val1, uint16_t val2, uint16_t val3)
    : TiffTag(id, TiffTag::Type::SHORT, 3, 0), m_values()
{
    m_values.push_back(val1);
    m_values.push_back(val2);
    m_values.push_back(val3);
    return;
}

inline ShortTag::ShortTag(TiffTag::ID id, const std::vector<uint16_t>& values)
    : TiffTag(id, TiffTag::Type::SHORT, 0, 0), m_values(values)
{
    m_tag.val.count = static_cast<uint32_t>(values.size());
    return;
}
