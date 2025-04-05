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

#include <string_view>

#include "Options.hpp"
#include "Logger.hpp"
namespace CmdLine {
class CmdLineParser;
};
class Image;

class RawDev {
public:
    static constexpr std::string_view k_compileDate{__DATE__};
    static constexpr std::string_view k_compileYear{k_compileDate.substr(7)};
    static_assert(k_compileYear.length() == 4);

    // Verbose output logger
    static Logger verbout;

    int run(int argc, const char* argv[]);

private:
    void processCmdLine(int argc, const char* argv[]);

    static void SetupOptions(CmdLine::Parser& parser);
    static void PrintHelp(const CmdLine::Parser& parser);
    static void PrintVersion();
    static void PrintLogo();
    static void PrintErrorSummary(int errorCount);

    bool loadRawImage(Image& img);
    void printProcessingSummary(const Image& img);

    Options m_options; // Program options
};
