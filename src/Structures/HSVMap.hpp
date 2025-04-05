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

#include <cassert>
#include <memory>

#include "Color.hpp"

class HSVMap {
public:
    // Scale HSV in double precission
    struct HSV64Scale {
        double hueShift, satScale, valScale;
    };

    HSVMap(int hueDim, int satDim, int valDim, const HSV64Scale* data);
    HSVMap(int hueDim, int satDim, int valDim,
        const HSV64Scale* data1, double illu1,
        const HSV64Scale* data2, double illu2,
        double temperature);
    void transform(Color::HSV64& hsv) const;

private:
    [[nodiscard]] HSV64Scale map(int h, int s, int v) const;
    [[nodiscard]] HSV64Scale interSlice(int, int, int, double, double) const;
    static void InterScales(HSV64Scale&, const HSV64Scale&, double);
    static void MakeOrderedIllu(const HSV64Scale*& data1, double& illu1,
        const HSV64Scale*& data2, double& illu2);
    static void Scale(const HSV64Scale& scale, Color::HSV64& val);

    int m_hueDim, m_satDim, m_valDim;
    std::unique_ptr<HSV64Scale[]> m_mapData;
};

////////////////////////////////////////////////////////////////////////////////

inline HSVMap::HSV64Scale HSVMap::map(int h, int s, int v) const
{
    assert(h >= 0 && h < m_hueDim);
    assert(s >= 0 && s < m_satDim);
    assert(v >= 0 && v < m_valDim);

    const int offset = m_satDim * (v * m_hueDim + h) + s;
    return m_mapData[offset];
}
