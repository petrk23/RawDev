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

#include "ByteTag.hpp"
using namespace std;

/*
Contruction from vector
*/
ByteTag::ByteTag(TiffTag::ID id, const vector<char>& values)
    : TiffTag(id, TiffTag::Type::BYTE, 0, 0), m_values(values)
{
    m_tag.val.count = static_cast<uint32_t>(values.size());

    // Special case for ICC tag
    if (id == TiffTag::ID::ICC) {
        m_tag.val.type = TiffTag::Type::UNDEFINDED;
    }
    return;
}

/*
Contruction from array
*/
ByteTag::ByteTag(TiffTag::ID id, const char* values, size_t count)
    : TiffTag(id, TiffTag::Type::BYTE, 0, 0), m_values()
{
    m_tag.val.count = static_cast<uint32_t>(count);
    for (size_t c = 0; c < count; c++) {
        m_values.push_back(values[c]);
    }

    // Special case for ICC tag
    if (id == TiffTag::ID::ICC) {
        m_tag.val.type = TiffTag::Type::UNDEFINDED;
    }
    return;
}

/*
Write byte tag to file
*/
void ByteTag::write(
    ofstream& file, uint32_t& offset, std::vector<char>& extraBytes)
{
    const size_t mem = extra();

    if (mem > 0) {
        m_tag.val.offset = offset;
        for (char& val : m_values) {
            extraBytes.push_back(val);
        }
        offset += static_cast<uint32_t>(mem);
    }
    else {
        m_tag.val.offset = 0; size_t pos = 0;
        for (char& v : m_values) {
            m_tag.mem[8 + pos++] = v;
        }
    }
    TiffTag::writeTag(file);
    return;
}

/*
Make clone of byte tag
*/
TiffTag* ByteTag::clone() const
{
    return new ByteTag(m_tag.val.id, m_values);
}

/*
Get extra data memory for tag in bytes
*/
size_t ByteTag::extra() const
{
    const size_t valueLenght = m_values.size();
    if (valueLenght >= 4)
        return valueLenght;
    return 0;
}
