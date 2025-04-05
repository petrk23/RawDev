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

#include "pch.hpp"
#include "Structures/Rect.hpp"

TEST(RectTest, DefaultConstructionTest)
{
    constexpr Rect r0;
    EXPECT_TRUE(r0.left == 0 && r0.top == 0 && r0.right == 0 && r0.bottom == 0);
}

TEST(RectTest, ConstructionTest)
{
    constexpr Rect r(Point(0, 10), Point(20, 30));
    EXPECT_TRUE(
        r.left == 0 &&
        r.top == 10 &&
        r.right == 20 &&
        r.bottom == 30);
}

TEST(RectTest, FactoryTest)
{
    constexpr Rect r1(Point(0, 10), Point(20, 30));
    constexpr Rect r2 = Rect::Create(Point(0, 10), 20, 20);
    EXPECT_TRUE(
        r1.left == r2.left &&
        r1.top == r2.top &&
        r1.right == r2.right &&
        r1.bottom == r2.bottom);
}

TEST(RectTest, DimensionTest)
{
    constexpr Rect r(Point(10, 20), Point(30, 40));

    constexpr auto width = r.getWidth();
    constexpr auto height = r.getHeight();
    EXPECT_EQ(width, 20);
    EXPECT_EQ(height, 20);
}
