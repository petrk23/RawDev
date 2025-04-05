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

#include "ShortTag.hpp"
using namespace std;

/*
Write short tag to file
*/
void ShortTag::write(ofstream& file,
    uint32_t& offset, vector<char>& extraBytes)
{
    const size_t mem = extra();

    if (mem > 0) {
        m_tag.val.offset = offset;

        for (auto& val : m_values) {
            const auto* p = reinterpret_cast<char*>(&val);
            extraBytes.push_back(p[0]);
            extraBytes.push_back(p[1]);
        }
        offset += static_cast<uint32_t>(mem);
    }
    else {
        m_tag.val.offset = 0; size_t pos = 0;
        auto* p = reinterpret_cast<uint16_t*>(m_tag.mem);
        for (auto& v : m_values) {
            p[4 + pos] = v; pos++;
        }
    }
    TiffTag::writeTag(file);
    return;
}

/*
Make clone of short tag
*/
TiffTag* ShortTag::clone() const
{
    return new ShortTag(m_tag.val.id, m_values);
}

/*
Get extra data memory for tag in bytes
*/
size_t ShortTag::extra() const
{
    const size_t valueLenght = m_values.size();
    if (valueLenght >= 2)
        return valueLenght * sizeof(uint16_t);
    return 0;
}
