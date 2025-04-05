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

#include <algorithm>
#include <cmath>
#include <memory>

#include "NonCopyable.hpp"
#include "Color.hpp"
#include "Array2D.hpp"

class CamProfile;
class CR2Reader;
class Path;

struct Mat3x3;
struct Rect;

class Image : NonCopyable {
public:
    enum class Channel : int {
        GREEN = 0, RED = 1, BLUE = 3
    };

    Image() = default;
    Image(const Image&);
    void loadCR2(const Path& inputFile, double temp);

    Color::RGB64 getValue(int, int) const;
    void setValue(int, int, Color::RGB64);
    double getValueR(int, int) const;
    double getValueG(int, int) const;
    double getValueB(int, int) const;
    double getValueX(int, int, Channel) const;
    std::shared_ptr<CamProfile> getCamProfile() const;

    int getWidth(void) const;
    int getHeight(void) const;

    void convert16(Array2D<Color::RGB16>& img16, bool noCrop) const;
    void convert8(Array2D<Color::RGB8>& img8, bool noCrop) const;
    static double clipDouble(double);

private:
    void setupMetadata(const CR2Reader& file, double temp);
    void storeRawData(Array2D<uint16_t>& img);
    static uint16_t doubleTo16(const double val);
    static uint8_t doubleTo8(const double val);

    Array2D<double> m_red, m_green, m_blue;
    std::shared_ptr<CamProfile> m_CamProfile;
};

///////////////////////////////////////////////////////////////////////////////

inline Image::Image(const Image& src)
    : m_red(src.m_red), m_green(src.m_green), m_blue(src.m_blue),
      m_CamProfile(src.m_CamProfile)
{
}

inline Color::RGB64 Image::getValue(int row, int col) const
{
    assert(row >= 0 && row < getHeight());
    assert(col >= 0 && col < getWidth());

    Color::RGB64 value{m_red[row][col], m_green[row][col], m_blue[row][col]};
    return value;
}

inline double Image::getValueR(int row, int col) const
{
    assert(row >= 0 && row < getHeight());
    assert(col >= 0 && col < getWidth());
    return m_red[row][col];
}

inline double Image::getValueG(int row, int col) const
{
    assert(row >= 0 && row < getHeight());
    assert(col >= 0 && col < getWidth());
    return m_green[row][col];
}

inline double Image::getValueB(int row, int col) const
{
    assert(row >= 0 && row < getHeight());
    assert(col >= 0 && col < getWidth());
    return m_blue[row][col];
}

/*
Return sensor value from bayer pattern
*/
inline double Image::getValueX(int row, int col, Channel ch) const
{
    assert(row >= 0 && row < getHeight());
    assert(col >= 0 && col < getWidth());
    switch (ch) {
    case Channel::RED:
        return m_red[row][col];
    case Channel::BLUE:
        return m_blue[row][col];
    default:
        break;
    }
    return m_green[row][col];
}

inline std::shared_ptr<CamProfile> Image::getCamProfile() const
{
    return m_CamProfile;
}

inline void Image::setValue(int row, int col, Color::RGB64 value)
{
    assert(row >= 0 && row < getHeight());
    assert(col >= 0 && col < getWidth());

    m_red[row][col] = Image::clipDouble(value.r);
    m_green[row][col] = Image::clipDouble(value.g);
    m_blue[row][col] = Image::clipDouble(value.b);
    return;
}

inline int Image::getWidth(void) const
{
    assert(m_red.getWidth() == m_green.getWidth() &&
           m_green.getWidth() == m_blue.getWidth());
    return m_red.getWidth();
}

inline int Image::getHeight(void) const
{
    assert(m_red.getHeight() == m_green.getHeight() &&
           m_green.getHeight() == m_blue.getHeight());
    return m_red.getHeight();
}

inline double Image::clipDouble(double value)
{
    return std::max(0.0, std::min(value, 1.0));
}

inline uint16_t Image::doubleTo16(const double val)
{
    return static_cast<uint16_t>(floor(65535.0 * val));
}

inline uint8_t Image::doubleTo8(const double val)
{
    return static_cast<uint8_t>(floor(255.0 * val));
}
