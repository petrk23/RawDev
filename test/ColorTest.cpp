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
#include "Color.hpp"

using namespace Color;

TEST(ColorTest, TestUVtoXYZ)
{
    constexpr CIEuv uv{0.35, 0.40};

    // Reference calculation.
    constexpr double x = 1.5 * uv.u / (uv.u - 4.0 * uv.v + 2.0);
    constexpr double y = uv.v / (uv.u - 4.0 * uv.v + 2.0);
    constexpr CIEXYZ ref{x / y, 1.0, (1 - x - y) / y};

    // Current calculation.
    const CIEXYZ target = uvToXYZ(uv, 1.0);

    // Compare the results
    constexpr double tolerance = 1e-15;
    EXPECT_NEAR(ref.x, target.x, tolerance);
    EXPECT_NEAR(ref.y, target.y, tolerance);
    EXPECT_NEAR(ref.z, target.z, tolerance);
}
