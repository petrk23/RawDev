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

#include "StringTag.hpp"
using namespace std;

/*
Write string tag to file
*/
void StringTag::write(ofstream& file,
    uint32_t& offset, vector<char>& extraBytes)
{
    const size_t mem = extra();

    if (mem > 0) {
        m_tag.val.offset = offset;
        for (char& c : m_value) {
            extraBytes.push_back(c);
        }
        extraBytes.push_back('\0');
        offset += static_cast<uint32_t>(mem);
    }
    else {
        m_tag.val.offset = 0; // Make zero string
        size_t pos = 0;
        for (char& c : m_value) {
            m_tag.mem[8 + pos++] = c;
        }
    }
    TiffTag::writeTag(file);
    return;
}

/*
Make clone of string tag
*/
TiffTag* StringTag::clone() const
{
    return new StringTag(m_tag.val.id, m_value);
}

/*
Get extra data memory for tag in bytes
*/
size_t StringTag::extra() const
{
    const size_t valueLenght = m_value.length();
    if (valueLenght >= 4)
        return valueLenght + 1;
    return 0;
}
