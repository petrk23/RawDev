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
#include "Utils.hpp"
#include "Structures/Mat3x3.hpp"

// Zero metrix
constexpr Mat3x3 k_zeroMat;

// Interpolation matrices
constexpr Mat3x3 k_intMat = {
    {{10, 20, 30},
     {40, 50, 60},
     {70, 80, 90}}
};
constexpr Mat3x3 k_intMatRef = {
    {{5, 10, 15},
     {20, 25, 30},
     {35, 40, 45}}
};

////////////////////////////////////////////////////////////////////////////////

TEST(UtilsTest, EvenOddTest)
{
    EXPECT_TRUE(Utils::even(0));
    EXPECT_TRUE(Utils::odd(1));
    EXPECT_TRUE(Utils::even(2));
    EXPECT_TRUE(Utils::odd(3));
    EXPECT_TRUE(Utils::even(4));
    EXPECT_TRUE(Utils::odd(5));
}

TEST(UtilsTest, IncAverageTest)
{
    constexpr int maxNumber = 5;
    double ave = 0;

    for (int n = 1; n <= maxNumber; ++n) {
        ave = Utils::incAverage(ave, n, n);
    }
    EXPECT_DOUBLE_EQ(ave, 3.0);
}

TEST(UtilsTest, SqrTest)
{
    EXPECT_EQ(Utils::sqr(5), 25);
    EXPECT_EQ(Utils::sqr(-5), 25);
}

TEST(UtilsTest, Val2EvTest)
{
    EXPECT_EQ(Utils::val2EV(1024), 10.0);
}

TEST(UtilsTest, Ev2ValTest)
{
    EXPECT_EQ(Utils::EV2Val(10), 1024.0);
}

TEST(UtilsTest, CompareDoubleTest)
{
    const double a = 10.0, b = 20.0;

    EXPECT_EQ(Utils::compareDouble(&a, &b), -1);
    EXPECT_EQ(Utils::compareDouble(&a, &a), 0);
    EXPECT_EQ(Utils::compareDouble(&b, &b), 0);
    EXPECT_EQ(Utils::compareDouble(&b, &a), 1);
}

TEST(UtilsTest, LinearInterpolationTest)
{
    // First variant
    EXPECT_DOUBLE_EQ(Utils::linearInter(0, 0, 0, 10, 10), 0.0);
    EXPECT_DOUBLE_EQ(Utils::linearInter(5, 0, 0, 10, 10), 5.0);
    EXPECT_DOUBLE_EQ(Utils::linearInter(7, 0, 0, 10, 10), 7.0);
    EXPECT_DOUBLE_EQ(Utils::linearInter(10, 0, 0, 10, 10), 10.0);

    // Second variant
    EXPECT_DOUBLE_EQ(Utils::linearInter(20, 100, .75), 40);
}

TEST(UtilsTest, InterpolateMatricesTest)
{
    EXPECT_EQ(Utils::interpolateMatrices(
        -1'000, k_zeroMat, 0, k_intMat, 10'000), k_zeroMat);
    EXPECT_EQ(Utils::interpolateMatrices(
        0, k_zeroMat, 0, k_intMat, 10000), k_zeroMat);
    EXPECT_EQ(Utils::interpolateMatrices(
        5'000, k_zeroMat, 0, k_intMat, 10'000), k_intMatRef);
    EXPECT_EQ(Utils::interpolateMatrices(
        10'000, k_zeroMat, 0, k_intMat, 10'000), k_intMat);
    EXPECT_EQ(Utils::interpolateMatrices(
        20'000, k_zeroMat, 0, k_intMat, 10'000), k_intMat);
}

TEST(UtilsTest, InterColorMatTest)
{
    EXPECT_EQ(Utils::interColorMat(
        5000, k_zeroMat, 5'000, k_zeroMat, 5'000), k_zeroMat);
    EXPECT_EQ(Utils::interColorMat(
        1e6 / 15, k_zeroMat, 50'000, k_intMat, 100'000), k_intMatRef);
    EXPECT_EQ(Utils::interColorMat(
        1e6 / 15, k_zeroMat, 100'000, k_intMat, 50'000), k_intMatRef);
    EXPECT_EQ(Utils::interColorMat(
        1, k_zeroMat, 50'000, k_intMat, 100'000), k_zeroMat);
    EXPECT_EQ(Utils::interColorMat(
        200'000, k_zeroMat, 50'000, k_intMat, 100'000), k_intMat);
}

TEST(UtilsTest, MemoryCopyTest)
{
    constexpr size_t arrDimen = 64;
    char values[arrDimen], targets[arrDimen];

    // Initialize and copy
    for (size_t i = 0; i < arrDimen; ++i) {
        values[i] = static_cast<char>(i);
    }
    Utils::memoryCopy(targets, values, sizeof(char) * arrDimen);

    // Check
    for (size_t i = 0; i < arrDimen; ++i) {
        EXPECT_EQ(values[i], targets[i]);
    }
}

TEST(UtilsTest, ByteSwapU16Test)
{
    constexpr uint16_t val = 0xABCD, res = 0xCDAB;
    EXPECT_EQ(Utils::byteSwapU16(val), res);
}
