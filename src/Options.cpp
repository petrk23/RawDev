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
#include "Options.hpp"
#include "CmdLineParser.hpp"

#include <sstream>
using namespace std;

/// <summary>
/// Process options from the command line
/// </summary>
/// <param name="parser">Cmd line parser for get values</param>
/// <returns>Number of errors in the options</returns>
int Options::process(const CmdLine::Parser &parser)
{
	// Simple switches
	assert(parser.foundSwitch("h") == false);
	assert(parser.foundSwitch("V") == false);
	m_NoCrop = parser.foundSwitch("u");
	m_NoProcess = parser.foundSwitch("x");
	m_Verbose = parser.foundSwitch("v");

	// Rest of the parameters
	return processInputFile(parser)
		+ processOutputFile(parser)
		+ processTint(parser)
		+ processContrast(parser)
		+ processDemosaicIter(parser)
		+ processTemperature(parser)
		+ processExposure(parser)
		+ processDemosaicAlg(parser)
		+ processBitDepth(parser)
		+ processColorProfile(parser);
}

/// <summary>
/// Setup input file from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processInputFile(const CmdLine::Parser &parser)
{
	string inputFile; // Get the input file
	const size_t paramCount = parser.getParamCount();
	int errorCount = 0;

	if (paramCount > 0)
	{
		if (paramCount > 1)
		{
			CmdLine::Parser::error(-1, 0, "Too much input files.");
			errorCount++;
		}
		m_InputFile = parser.getParam(0);
		if (m_InputFile.getExtension() != "cr2" &&
			m_InputFile.getExtension() != "CR2")
		{
			CmdLine::Parser::error(-1, 0,
				"Input file must have extension cr2 or CR2.");
			errorCount++;
		}
	}
	else
	{
		CmdLine::Parser::error(-1, 0, "They are no files to be processed.");
		errorCount++;
	}
	return errorCount;
}

/// <summary>
/// Setup output file from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processOutputFile(const CmdLine::Parser &parser)
{
	string outputFile; // Get the output file

	if (parser.found("o", outputFile))
		m_OutputFile = outputFile;
	else
	{
		m_OutputFile = m_InputFile;
		m_OutputFile.setExtension(".tif");
	}
	return 0;
}

/// <summary>
/// Setup tint option from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processTint(const CmdLine::Parser &parser)
{
	int tint; // Load tint value
	const int found = parser.found("t", tint);
	if (found > 0)
	{
		if (tint < -100 || tint > 100)
		{
			CmdLine::Parser::error(found, -1, "Tint value is out of range.");
			return 1;
		}
		m_Tint = tint;
	}
	return 0;
}

/// <summary>
/// Setup contrast option from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processContrast(const CmdLine::Parser &parser)
{
	int contrast;
	const int found = parser.found("c", contrast);
	if (found > 0)
	{
		if (contrast < -100 || contrast > 100)
		{
			CmdLine::Parser::error(found, -1, "Contrast value is out of range.");
			return 1;
		}
		m_Contrast = contrast;
	}
	return 0;
}

/// <summary>
/// Setup demosaic iterations from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processDemosaicIter(const CmdLine::Parser &parser)
{
	int demosaicIter;
	const int found = parser.found("i", demosaicIter);
	if (found > 0)
	{
		if (demosaicIter < 0 || demosaicIter > 10)
		{
			CmdLine::Parser::error(found, -1,
				"Demosaic iteration value is out of range.");
			return 1;
		}
		m_DemosaicIter = demosaicIter;
	}
	return 0;
}

/// <summary>
/// Setup color temperature from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processTemperature(const CmdLine::Parser &parser)
{
	double temp;
	const int found = parser.found("T", temp);
	if (found > 0)
	{
		if (temp < 2000.0 || temp > 15000.0)
		{
			CmdLine::Parser::error(found, -1,
				"Color temperature value is out of range.");
			return 1;
		}
		m_Temperature = temp;
	}
	return 0;
}

/// <summary>
/// Setup exposure from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processExposure(const CmdLine::Parser &parser)
{
	double exposure;
	const int found = parser.found("e", exposure);
	if (found > 0)
	{
		if (exposure < -5.0 || exposure > 5.0)
		{
			CmdLine::Parser::error(found, -1,
				"Exposure value is out of range.");
			return 1;
		}
		m_Exposure = exposure;
	}
	return 0;
}

/// <summary>
/// Setup desmosaic algorithm selection from cmd line
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processDemosaicAlg(const CmdLine::Parser &parser)
{
	string da;
	const int found = parser.found("d", da);
	if (found > 0)
	{
		if (da.compare("bilinear") == 0)
			m_DemosaicAlg = Demosaic::AlgorithmType::Bilinear;
		else if (da.compare("freeman") == 0)
			m_DemosaicAlg = Demosaic::AlgorithmType::Freeman;
		else if (da.compare("hqlinear") == 0)
			m_DemosaicAlg = Demosaic::AlgorithmType::HQLinear;
		else if (da.compare("ahd") == 0)
			m_DemosaicAlg = Demosaic::AlgorithmType::AHD;
		else
		{
			stringstream sstream;
			sstream << "Demosaicing algorithm '"
				<< da << "' in unknown.";
			CmdLine::Parser::error(found, -1, sstream.str());
			return 1;
		}
	}
	return 0;
}

/// <summary>
/// Processing bit depth options
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processBitDepth(const CmdLine::Parser& parser)
{
	int bitDepth;
	int pos = parser.found("b", bitDepth);

	if (pos > 0)
	{
		if (bitDepth == 8 || bitDepth == 16)
			m_bitDepth = bitDepth;
		else
		{
			CmdLine::Parser::error(pos, -1, "Only 8 or 16 bits allowed.");
			return 1;
		}
	}
	return 0;
}

/// <summary>
/// Process color profile selection
/// </summary>
/// <param name="parser">Cmd line parser</param>
/// <returns>Error count</returns>
int Options::processColorProfile(const CmdLine::Parser& parser)
{
	string profileName;
	int found = parser.found("p", profileName);

	if (found)
	{
		if (profileName.compare("srgb") == 0)
			m_colorProfile = ColorProfile::sRGB;
		else if (profileName.compare("argb") == 0)
			m_colorProfile = ColorProfile::aRGB;
		else
		{
			CmdLine::Parser::error(found, -1, "Unknown color profile.");
			return 1;
		}
	}
	return 0;
}
