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

#include "Structures/Point.hpp"

/*
We don't care about empty or negative sized rectangles.
That makes sense, as we implement a data structure to
working with. Last, but not least, this behavior has no
impact on the code correctness.
*/

struct Rect {
    int left, right, top, bottom;

    constexpr Rect() noexcept
        : Rect(Point(0, 0), Point(0, 0))
    {
    }

    constexpr Rect(Point leftTop, Point rightBottom) noexcept
        : left{leftTop.x}, right{rightBottom.x},
          top{leftTop.y},  bottom{rightBottom.y}
    {
    }

    [[nodiscard]] constexpr int getWidth() const noexcept;
    [[nodiscard]] constexpr int getHeight() const noexcept;

    // Factory method
    static constexpr Rect Create(Point origin, int width, int height) noexcept;
};

////////////////////////////////////////////////////////////////////////////////

constexpr Rect Rect::Create(Point origin, int width, int height) noexcept
{
    return {origin, Point(origin.x + width, origin.y + height)};
}

constexpr int Rect::getWidth() const noexcept
{
    return right - left;
}

constexpr int Rect::getHeight() const noexcept
{
    return bottom - top;
}
