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

#include "Structures/Path.hpp"
#include <string>

class Image;
class Options;

class OutputModule {
    Path m_InputFile, m_OutputFile;
    std::string m_Artist;

public:
    static void run(Image& img, const Options& opt);

private:
    OutputModule(const Options& opt);
    void process(Image& img, const Options& opt);

private: // Helpers
    std::string formatCopyright() const;
    static int getYear();
    static void conversionMessage(const char* profileName, const char* curveName);

private: // Gamma correction
    static void convert2argb(Image& img);
    static void convert2srgb(Image& img);
    static double srgbGammaCurve(double value);
};
