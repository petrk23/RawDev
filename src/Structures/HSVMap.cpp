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

#include "HSVMap.hpp"
#include "Utils.hpp"

/*
Initializer of the HSV map transformator
*/
HSVMap::HSVMap(int hueDim, int satDim, int valDim, const HSV64Scale* data)
    : m_hueDim(hueDim), m_satDim(satDim), m_valDim(valDim)
{
    int mapSize = hueDim * satDim * valDim;
    m_mapData = std::make_unique<HSV64Scale[]>(mapSize);
    mapSize *= sizeof(HSV64Scale); // Compute map size in bytes
    Utils::memoryCopy(m_mapData.get(), data, mapSize);
}

/*
Temperature interpolated map by color temperature
*/
HSVMap::HSVMap(
    int hueDim, int satDim, int valDim,
    const HSV64Scale* data1, double illu1,
    const HSV64Scale* data2, double illu2,
    double temperature)
    : m_hueDim(hueDim), m_satDim(satDim), m_valDim(valDim)
{
    int mapSize = hueDim * satDim * valDim;
    m_mapData = std::make_unique<HSV64Scale[]>(mapSize);
    HSVMap::MakeOrderedIllu(data1, illu1, data2, illu2);

    if (temperature <= illu1) {
        mapSize *= sizeof(HSV64Scale);
        Utils::memoryCopy(m_mapData.get(), data1, mapSize);
    }
    else if (temperature >= illu2) {
        mapSize *= sizeof(HSV64Scale);
        Utils::memoryCopy(m_mapData.get(), data2, mapSize);
    }
    else {
        illu1 = Color::kelvin2mired(illu1),
        illu2 = Color::kelvin2mired(illu2);
        temperature = Color::kelvin2mired(temperature);

        for (int c = 0; c < mapSize; c++) {
            HSV64Scale val; // Final interpolated value
            val.hueShift = Utils::linearInter(temperature,
                illu1, data1[c].hueShift, illu2, data2[c].hueShift);
            val.satScale = Utils::linearInter(temperature,
                illu1, data1[c].satScale, illu2, data2[c].satScale);
            val.valScale = Utils::linearInter(temperature,
                illu1, data1[c].valScale, illu2, data2[c].valScale);
            m_mapData[c] = val;
        }
    }
}

/*
Ordering data by illuminant that illu1 <= illu2
*/
inline void HSVMap::MakeOrderedIllu(
    const HSV64Scale*& data1, double& illu1,
    const HSV64Scale*& data2, double& illu2)
{
    if (illu1 > illu2) {
        std::swap(data1, data2);
        std::swap(illu1, illu2);
    }
    assert(illu1 <= illu2);
}

/*
Transformation with a camera profile
*/
void HSVMap::transform(Color::HSV64& hsv) const
{
    // Hue coordinates
    const auto lasthue = m_hueDim - 1;
    const int h = static_cast<int>(hsv.hue * lasthue);
    const double h0 = static_cast<double>(h) / lasthue;
    const double hd = (hsv.hue - h0) * m_hueDim;

    // Sat coordinates
    const auto lastsat = m_satDim - 1;
    const int s = static_cast<int>(hsv.sat * lastsat);
    const double s0 = static_cast<double>(s) / lastsat;
    const double sd = (hsv.sat - s0) * m_satDim;

    // Value coordinates
    const auto lastval = m_valDim - 1;
    const int v = static_cast<int>(hsv.val * lastval);
    const double v0 = static_cast<double>(v) / lastval;
    const double vd = (hsv.val - v0) * m_valDim;

    // Interpolate between slices of the HSV cube
    HSV64Scale trans = interSlice(h, s, v, hd, sd);
    if (v + 1 < m_valDim) {
        InterScales(trans, interSlice(h, s, v + 1, hd, sd), vd);
    }
    HSVMap::Scale(trans, hsv); // Apply scaling
}

/*
Apply scaling to the value
*/
void HSVMap::Scale(const HSV64Scale& scale, Color::HSV64& val)
{
    val.hue += scale.hueShift / 360;
    if (val.hue > 1.0) {
        val.hue -= 1.0;
    }
    else if (val.hue < 0.0) {
        val.hue += 1.0;
    }
    val.sat = std::clamp(val.sat * scale.satScale, 0.0, 1.0);
    val.val = std::clamp(val.val * scale.valScale, 0.0, 1.0);
}

/*
Bilinear interpolation on a HSV cube slice
*/
HSVMap::HSV64Scale
HSVMap::interSlice(int h, int s, int v, double hd, double sd) const
{
    HSV64Scale c00 = map(h, s, v);
    if (s + 1 < m_satDim) { // Interpolate (x direction)
        InterScales(c00, map(h, s + 1, v), sd);
    }
    h++; // Next hue (y shift)

    if (h < m_hueDim) {
        HSV64Scale c10 = map(h, s, v);
        if (s + 1 < m_satDim) { // Interpolate (x direction)
            InterScales(c10, map(h, s + 1, v), sd);
        }
        // Interpolate c00 and c10 (y direction)
        InterScales(c00, c10, hd);
    }
    return c00;
}

/*
Interpolate between two HSV scaling values
*/
inline void HSVMap::InterScales(
    HSV64Scale& sc0, const HSV64Scale& sc1, const double slope)
{
    sc0.hueShift = (sc0.hueShift + (sc1.hueShift - sc0.hueShift) * slope);
    sc0.satScale = (sc0.satScale + (sc1.satScale - sc0.satScale) * slope);
    sc0.valScale = (sc0.valScale + (sc1.valScale - sc0.valScale) * slope);
}
