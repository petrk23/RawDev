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
#include "Structures/Mat3x3.hpp"

// Inversible matrix, det = -42
constexpr Mat3x3 k_mat1 = {
    {{3, 0, 1}, {1, 2, 5}, {-1, 4, 2}}
};
// Not inversible, det = 0
constexpr Mat3x3 k_mat2 = {
    {{1, 2, 1}, {4, 5, 4}, {1, 2, 1}}
};

////////////////////////////////////////////////////////////////////////////////

TEST(Mat3x3Test, DefaltConstruct)
{
    constexpr Mat3x3 zero = {
        {{0, 0, 0},
         {0, 0, 0},
         {0, 0, 0}}
    };
    Mat3x3 mat;
    EXPECT_EQ(mat, zero);
}

TEST(Mat3x3Test, MultiplyTest)
{
    constexpr Mat3x3 multRef = {
        {{4,   8,  4},
         {14, 22, 14},
         {17, 22, 17}}
    };
    EXPECT_EQ(k_mat1 * k_mat2, multRef);
}

TEST(Mat3x3Test, VectorMultiplyTest)
{
    constexpr Color::RGB64
        v1 = {10, 20, 30},
        v1res = {60, 200, 130},
        v2res = {70, 130, 120};
    constexpr Color::CIEXYZ v2 = {20, 30, 10};

    // RGB vector
    const auto rgb1 = Color::rgbTo<Color::RGB64>(k_mat1, v1);
    EXPECT_EQ(rgb1.r, v1res.r);
    EXPECT_EQ(rgb1.g, v1res.g);
    EXPECT_EQ(rgb1.b, v1res.b);

    // RGB vector to XYZ (should be same as previous)
    const auto xyz1 = Color::rgbTo<Color::CIEXYZ>(k_mat1, v1);
    EXPECT_EQ(xyz1.x, v1res.r);
    EXPECT_EQ(xyz1.y, v1res.g);
    EXPECT_EQ(xyz1.z, v1res.b);

    // CIE XYZ vector
    const auto rgb2 = Color::xyzTo<Color::RGB64>(k_mat1, v2);
    EXPECT_EQ(rgb2.r, v2res.r);
    EXPECT_EQ(rgb2.g, v2res.g);
    EXPECT_EQ(rgb2.b, v2res.b);
}

TEST(Mat3x3Test, ScalarDivTest)
{
    Mat3x3 dekmat = {
        {{10, 100, 1000}, {5, 10, 100}, {50, 100, 50}}
    };
    constexpr Mat3x3 dekmatRef = {
        {{1, 10, 100}, {0.5, 1, 10}, {5, 10, 5}}
    };
    dekmat /= 10;
    EXPECT_EQ(dekmat, dekmatRef);
}

TEST(Mat3x3Test, DeterminantTest)
{
    EXPECT_EQ(Mat3x3::k_unitMatrix.det(), 1.0);
    EXPECT_EQ(k_mat1.det(), -42.0);
    EXPECT_EQ(k_mat2.det(), 0.0);
}

TEST(Mat3x3Test, InversionTest)
{
    // Unit matrix inversion is unit
    Mat3x3 unitInv = Mat3x3::k_unitMatrix.inverse();
    EXPECT_EQ(unitInv, Mat3x3::k_unitMatrix);

    // Matrix1 inversion reference
    constexpr Mat3x3 mat1InvRef = {
        {{ 8.0 / 21, -2.0 / 21,  1.0 / 21},
         { 1.0 /  6, -1.0 /  6,  1.0 /  3},
         {-1.0 /  7,  2.0 /  7, -1.0 /  7}}
    };
    Mat3x3 mat1Inv = k_mat1.inverse();
    EXPECT_EQ(mat1Inv, mat1InvRef);

    // Double inversion tests
    EXPECT_EQ(Mat3x3::k_unitMatrix.inverse().inverse(), Mat3x3::k_unitMatrix);
    EXPECT_EQ(k_mat1.inverse().inverse(), k_mat1);
}

TEST(Mat3x3Test, TestInversible)
{
    EXPECT_TRUE(Mat3x3::k_unitMatrix.isInversible());
    EXPECT_TRUE(k_mat1.isInversible());
    EXPECT_FALSE(k_mat2.isInversible());
}

TEST(Mat3x3Test, DiagonalTest)
{
    EXPECT_TRUE(Mat3x3::k_unitMatrix.isDiagonal());
    EXPECT_FALSE(k_mat1.isDiagonal());
    EXPECT_FALSE(k_mat2.isDiagonal());
}

TEST(Mat3x3Test, EqualityTest)
{
    EXPECT_TRUE(k_mat1 == k_mat1);
    EXPECT_TRUE(k_mat1 != k_mat2);
    EXPECT_FALSE(k_mat1 == k_mat2);
    EXPECT_FALSE(k_mat1 != k_mat1);
}
