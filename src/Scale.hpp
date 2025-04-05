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

#include <memory>
#include "Color.hpp"

class Image;
class Options;
class CamProfile;
struct Rect;

class ScaleModule
{
    std::shared_ptr<CamProfile> m_CamProfile;
    double m_ColorTemp;
    int m_Tint;

public:
    static void run(Image& img, const Options &opt);

private:
    ScaleModule(// Constructor
        const std::shared_ptr<CamProfile> &profile, const Options &opt);
    void scale(Image &img, double black,
        double scaleR, double scaleG, double scaleB);
    void process(Image &img);

private: // Black point helpers
    double calcBlackPoint(const Image &img);
    void averageBlacks(const Image &img,
        Rect(&masked)[2], Color::RGB64 &black);
    Color::RGB64 estimateBlackPoint(const Image &img);

private: // Other
    void printScale(const char* name,
        const double value, const bool last = false);
};
