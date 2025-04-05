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
#include "CamProfiles/CFAPattern.hpp"

class CFAPatternTest : public testing::Test {
protected:
    CFAPattern m_pat0, m_pat1, m_pat2, m_pat3;
    CFAPatternTest() : m_pat0(CFAPattern::Filter::RGGB),
                       m_pat1(CFAPattern::Filter::GBRG),
                       m_pat2(CFAPattern::Filter::BGGR),
                       m_pat3(CFAPattern::Filter::GRBG) {}
};

////////////////////////////////////////////////////////////////////////////////

TEST_F(CFAPatternTest, ConstructorTest)
{
    EXPECT_EQ(m_pat0.getFilter(), CFAPattern::Filter::RGGB);
    EXPECT_EQ(m_pat1.getFilter(), CFAPattern::Filter::GBRG);
    EXPECT_EQ(m_pat2.getFilter(), CFAPattern::Filter::BGGR);
    EXPECT_EQ(m_pat3.getFilter(), CFAPattern::Filter::GRBG);
}

TEST_F(CFAPatternTest, TestRGGB)
{
    EXPECT_EQ(m_pat0(0, 0), CFAPattern::Color::RED);
    EXPECT_EQ(m_pat0(0, 1), CFAPattern::Color::GREEN_R);
    EXPECT_EQ(m_pat0(1, 0), CFAPattern::Color::GREEN_B);
    EXPECT_EQ(m_pat0(1, 1), CFAPattern::Color::BLUE);
}

TEST_F(CFAPatternTest, TestGBRG)
{
    EXPECT_EQ(m_pat1(0, 0), CFAPattern::Color::GREEN_B);
    EXPECT_EQ(m_pat1(0, 1), CFAPattern::Color::BLUE);
    EXPECT_EQ(m_pat1(1, 0), CFAPattern::Color::RED);
    EXPECT_EQ(m_pat1(1, 1), CFAPattern::Color::GREEN_R);
}

TEST_F(CFAPatternTest, TestBGGR)
{
    EXPECT_EQ(m_pat2(0, 0), CFAPattern::Color::BLUE);
    EXPECT_EQ(m_pat2(0, 1), CFAPattern::Color::GREEN_B);
    EXPECT_EQ(m_pat2(1, 0), CFAPattern::Color::GREEN_R);
    EXPECT_EQ(m_pat2(1, 1), CFAPattern::Color::RED);
}

TEST_F(CFAPatternTest, TestGRBG)
{
    EXPECT_EQ(m_pat3(0, 0), CFAPattern::Color::GREEN_R);
    EXPECT_EQ(m_pat3(0, 1), CFAPattern::Color::RED);
    EXPECT_EQ(m_pat3(1, 0), CFAPattern::Color::BLUE);
    EXPECT_EQ(m_pat3(1, 1), CFAPattern::Color::GREEN_B);
}
