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
#include "Structures/Array2D.hpp"

constexpr int k_width = 10;
constexpr int k_height = 20;

template<typename T>
static void CheckDims(
    const Array2D<T>& arr, const int width, const int height)
{
    EXPECT_EQ(arr.getWidth(), width);
    EXPECT_EQ(arr.getHeight(), height);
}

static void FillWithSeries(Array2D<int>& arr)
{
    const int width = arr.getWidth(),
              height = arr.getHeight();

    // Fill with sequential numbers
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            arr[row][col] = row * width + col;
        }
    }
}

static void CheckSeries(const Array2D<int>& arr)
{
    const int width = arr.getWidth(),
              height = arr.getHeight();

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            EXPECT_EQ(arr[row][col], row * width + col);
        }
    }
}

template<typename T>
static void CheckValue(const Array2D<T>& arr, const T& value)
{
    const int width = arr.getWidth(),
              height = arr.getHeight();

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            EXPECT_EQ(arr[row][col], value);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

TEST(Array2DTest, DefaltConstructTest)
{
    Array2D<int> arr;
    CheckDims(arr, 0, 0);
    EXPECT_TRUE(arr.empty());
}

TEST(Array2DTest, ConstructTest)
{
    // Normal usage
    Array2D<int> arr(k_width, k_height);
    CheckDims(arr, k_width, k_height);
    FillWithSeries(arr);
    CheckSeries(arr);

    // Empty fallback
    EXPECT_THROW(Array2D<int> x(10, -5), std::out_of_range);
}

TEST(Array2DTest, InitConstructTest)
{
    constexpr int initValue = 1212;

    // Trivial type
    Array2D<int> arr(k_width, k_height, initValue);
    CheckDims(arr, k_width, k_height);
    CheckValue(arr, initValue);

    // Complex type (std::string)
    const std::string strInitValue("Test123");
    Array2D<std::string> arr2(k_width, k_height, strInitValue);
    CheckDims(arr2, k_width, k_height);
    CheckValue(arr2, strInitValue);
}

TEST(Array2DTest, CopyConstructTest)
{
    Array2D<int> arr(k_width, k_height);
    FillWithSeries(arr);
    Array2D<int> copy(arr);
    CheckDims(copy, arr.getWidth(), arr.getHeight());
    CheckDims(copy, k_width, k_height);
    CheckSeries(copy);
}

TEST(Array2DTest, MoveConstructTest)
{
    // Simple move
    Array2D<int> arr = Array2D<int>(k_width, k_height);
    FillWithSeries(arr);
    CheckDims(arr, k_width, k_height);

    // Move from STD
    Array2D<int> x = std::move(arr);
    CheckDims(x, k_width, k_height);
    CheckSeries(x);
    EXPECT_EQ(arr.getWidth(), 0); // Move constructor made original empty
    EXPECT_EQ(arr.getHeight(), 0);
    EXPECT_TRUE(arr.empty());
}

TEST(Array2DTest, AssignMoveTest)
{
    Array2D<int> arr;
    arr = Array2D<int>(k_width, k_height);
    CheckDims(arr, k_width, k_height);

    Array2D<int> arr2(k_width, k_height);
    FillWithSeries(arr2);
    arr = std::move(arr2);
    CheckSeries(arr);
}

TEST(Array2DTest, SelectionTest)
{
    // Default constructed
    Array2D<int> arr1;
    // arr1[5][5] = 5; // fails as nothing initialized

    // Constructed with sizes
    Array2D<int> arr2(k_width, k_height);
    FillWithSeries(arr2);
    CheckSeries(arr2);
}

TEST(Array2DTest, InsideTest)
{
    Array2D<int> arr(k_width, k_height);

    // All possible cases
    EXPECT_TRUE(arr.inside(0, 0));
    EXPECT_TRUE(arr.inside(k_height - 1, 0));
    EXPECT_TRUE(arr.inside(0, k_width - 1));
    EXPECT_FALSE(arr.inside(k_height, 0));
    EXPECT_FALSE(arr.inside(0, k_width));
    EXPECT_FALSE(arr.inside(k_height, k_width));

    // Negative indexes
    EXPECT_FALSE(arr.inside(-1, 0));
    EXPECT_FALSE(arr.inside(0, -1));
    EXPECT_FALSE(arr.inside(-1, -1));
}
