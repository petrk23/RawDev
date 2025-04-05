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

#include "RationalTag.hpp"
using namespace std;

/*
Write relational tag to file
*/
void RationalTag::write(ofstream& file,
    uint32_t& offset, vector<char>& extraBytes)
{
    m_tag.val.offset = offset;
    offset += static_cast<uint32_t>(extra());

    // Disassembly numarator
    auto* p = reinterpret_cast<char*>(&m_num);
    extraBytes.push_back(p[0]);
    extraBytes.push_back(p[1]);
    extraBytes.push_back(p[2]);
    extraBytes.push_back(p[3]);

    // Disassembly denominator
    p = reinterpret_cast<char*>(&m_den);
    extraBytes.push_back(p[0]);
    extraBytes.push_back(p[1]);
    extraBytes.push_back(p[2]);
    extraBytes.push_back(p[3]);

    // Write tag
    TiffTag::writeTag(file);
    return;
}

/*
Make clone of relational tag
*/
TiffTag* RationalTag::clone() const
{
    return new RationalTag(m_tag.val.id, m_num, m_den);
}

/*
Get extra data memory for tag in bytes
*/
size_t RationalTag::extra() const
{
    return 2 * sizeof(uint32_t);
}
