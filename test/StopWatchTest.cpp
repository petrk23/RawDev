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
#include "StopWatch.hpp"

TEST(StopWatchTest, ConstructionTest)
{
    StopWatch s1, s2(false), s3(true);
    EXPECT_FALSE(s1.running());
    EXPECT_FALSE(s2.running());
    EXPECT_TRUE(s3.running());
}

TEST(StopWatchTest, RunningTest)
{
    StopWatch s;

    EXPECT_FALSE(s.running());
    s.start();
    EXPECT_TRUE(s.running());
    s.stop();
    const double val = s.currTime();
    EXPECT_FALSE(s.running());
    EXPECT_GE(val, 0.0);
    s.stop();
    EXPECT_FALSE(s.running());
    EXPECT_EQ(s.currTime(), val);
}

TEST(StopWatchTest, ResetTest)
{
    StopWatch s;

    s.start();
    EXPECT_TRUE(s.running());
    s.reset();
    EXPECT_FALSE(s.running());
    EXPECT_EQ(s.currTime(), 0.0);
}

TEST(StopWatchTest, CurrentTimeTest)
{
    StopWatch s;

    // Before start
    EXPECT_FALSE(s.running());
    EXPECT_DOUBLE_EQ(s.currTime(), 0.0);

    // Start
    s.start();
    EXPECT_TRUE(s.running());
    EXPECT_GE(s.currTime(), 0.0);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // First second
    EXPECT_GE(s.currTime(), 1.0);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Continuous readout
    std::vector<double> td{
        s.currTime(), s.currTime(), s.currTime(), s.currTime()
    };
    EXPECT_TRUE(std::is_sorted(td.begin(), td.end()));

    // Stop
    s.stop();
    double tmp = s.currTime();
    EXPECT_FALSE(s.running());
    EXPECT_EQ(tmp, s.currTime());
}

TEST(StopWatchTest, TimeMeasureTest)
{
    StopWatch s(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1234));
    s.stop();
    EXPECT_FALSE(s.running());
    EXPECT_GE(s.currTime(), 1.234);
}
