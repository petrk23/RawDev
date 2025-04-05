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

class UtilsTestStat3 : public testing::Test {
protected:
    static constexpr int k_testCount = 64;
    enum class RefIndex { Min, Median, Max };

    // Random number generation
    std::default_random_engine m_generator;
    std::uniform_int_distribution<> m_distribution;

    std::array<int, 3> m_data{};
    void genData();
    void check(RefIndex ri, std::function<int(int, int, int)> func);
};

void UtilsTestStat3::genData()
{
    for (auto& val : m_data) {
        val = m_distribution(m_generator); // Generate data
    }
}

void UtilsTestStat3::check(
    const RefIndex ri, std::function<int(int, int, int)> func)
{
    for (int i = 0; i < k_testCount; ++i) {
        const int med = func(m_data[0], m_data[1], m_data[2]);
        std::sort(m_data.begin(), m_data.end()); // Make reference data
        EXPECT_EQ(med, m_data[static_cast<int>(ri)]);
        genData(); // Regenerate data
    }
}

////////////////////////////////////////////////////////////////////////////////

TEST_F(UtilsTestStat3, Min3Test)
{
    check(RefIndex::Min, Utils::min3<int>);
}

TEST_F(UtilsTestStat3, Max3Test)
{
    check(RefIndex::Max, Utils::max3<int>);
}

TEST_F(UtilsTestStat3, MedianTest)
{
    check(RefIndex::Median, Utils::median<int>);
}

TEST_F(UtilsTestStat3, MedianXorTest)
{
    check(RefIndex::Median, Utils::medianXor<int>);
}
