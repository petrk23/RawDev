/*
 * This file is part of RawDev.
 *
 * Copyright (C) 2020-2023 Petr Krajník
 *
 * RawDev is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RawDev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RawDev.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "Options.hpp"
#include "Logger.hpp"
namespace CmdLine { class CmdLineParser; };
class Image;

class RawDev
{
	Options m_Options;     // Program options

public:
	static Logger verbout; // Verbose output logger

public:
	RawDev() = default; ~RawDev() = default;
	int run(int argc, const char *argv[]);

private: // Command line parsing
	void processCmdLine(int argc, const char *argv[]);
	void setupOptions(CmdLine::Parser &parser);
	void printLogo() const;
	void printHelp(const CmdLine::Parser &parser) const;
	void printVersion() const;
	void printProcessingSummary(const Image &img) const;
	void printErrorSummary(int errorCount) const;
	bool loadRawImage(Image &img);
};
