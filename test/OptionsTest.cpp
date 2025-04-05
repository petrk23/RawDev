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
#include "CmdLineParser.hpp"
#include "Options.hpp"

TEST(OptionsTest, DefaultsTest)
{
    constexpr double tolerance = 1e-15;
    const Options opt;

    EXPECT_EQ(opt.getInputFile().getPath(), "");
    EXPECT_EQ(opt.getOutputFile().getPath(), "output.tif");
    EXPECT_EQ(opt.getDemosaicAlg(), Demosaic::AlgorithmType::AHD);
    EXPECT_EQ(opt.getDemosaicIter(), 3);
    EXPECT_EQ(opt.getColorProfile(), ColorProfile::sRGB);
    EXPECT_EQ(opt.getBitDepth(), 8);
    EXPECT_NEAR(opt.getTemperature(), 5000, tolerance);
    EXPECT_EQ(opt.getTint(), 0);
    EXPECT_NEAR(opt.getExposure(), 0, tolerance);
    EXPECT_EQ(opt.getContrast(), 25);
    EXPECT_EQ(opt.getNoCrop(), false);
    EXPECT_EQ(opt.getNoProcess(),false);
    EXPECT_EQ(opt.getVerbose(), false);
    EXPECT_EQ(opt.getArtistName(), std::string(""));
}

TEST(OptionsTest, InputFileTest)
{
    const char* fileName = "cosi.cr2";
    const char* args[] = {"exe", fileName};
    constexpr int argc = sizeof(args) / sizeof(char*);

    CmdLine::Parser p;
    p.parse(argc, args);

    Options opt;
    const int errors = opt.process(p);
    EXPECT_EQ(errors, 0);
    EXPECT_EQ(opt.getInputFile().getPath(), fileName);
}
