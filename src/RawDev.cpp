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
#include "RawDev.hpp"

#include <iostream>
#include <cstdlib>

#include "Structures/Image.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "Options.hpp"
#include "CmdLineParser.hpp"
#include "StopWatch.hpp"
#include "Exception.hpp"
#include "Version.hpp"

#include "Scale.hpp"
#include "Denoise.hpp"
#include "Demosaic.hpp"
#include "ProcRGB.hpp"
#include "Output.hpp"

using namespace std;

/// <summary>
/// Verbose output logger.
/// </summary>
Logger RawDev::verbout(cout);

/// <summary>
/// Run RawDev application
/// </summary>
/// <param name="argc">Command line argument count</param>
/// <param name="argv">Command line arguments</param>
/// <returns>Application run result code</returns>
int RawDev::run(int argc, const char *argv[])
{
	StopWatch watch(true);

	processCmdLine(argc, argv); // Process cmd line options
	verbout.setEnabled(m_Options.getVerbose());

	Image img; // Raw image for processing
	if (loadRawImage(img) == false)
	{
		cerr << endl; // Empty line before this summary
		cerr << "Failed to read input RAW file. END." << endl;
		return EXIT_FAILURE;
	}
	printProcessingSummary(img); // Values of processing options

	verbout.indent(); // Go to itemize mode
	verbout << endl;

	verbout << "Scaling colors in camera native space" << endl;
	verbout.indent();
	ScaleModule::run(img, m_Options);
	verbout.unindent();

	verbout << "Denoising RAW image data" << endl;
	verbout.indent();
	DenoiseModule::run(img, m_Options);
	verbout.unindent();

	verbout << "Demosaicing pixels on the Bayer mask" << endl;
	verbout.indent();
	DemosaicModule::run(img, m_Options);
	verbout.unindent();

	verbout << "Processing RGB image" << endl;
	verbout.indent();
	ProcRGBModule::run(img, m_Options);
	verbout.unindent();

	verbout << "Finishing and output" << endl;
	verbout.indent();
	OutputModule::run(img, m_Options);
	verbout.unindent();

	watch.stop(); // Stop measurement
	verbout << endl;
	cout << "DONE in " << watch << "." << endl;

	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Process command line options
/// </summary>
/// <param name="argc">Argument count</param>
/// <param name="argv">Argument values</param>
void RawDev::processCmdLine(int argc, const char *argv[])
{
	CmdLine::Parser parser;
	setupOptions(parser); // Setup available options

	// Parse the command line
	int errorCount = parser.parse(argc, argv);
	if (errorCount == 0)
	{
		if (parser.foundSwitch("h") == true)
		{
			printHelp(parser);
			exit(EXIT_SUCCESS);
		}
		else if (parser.foundSwitch("V") == true)
		{
			printVersion();
			exit(EXIT_SUCCESS);
		}
		else errorCount = m_Options.process(parser);
	}
	printErrorSummary(errorCount);
	return;
}

/// <summary>
/// Setup app available options
/// </summary>
/// <param name="parser">Command line argument parser</param>
void RawDev::setupOptions(CmdLine::Parser &parser)
{
	// Development options
	parser.addGroup("Develop options");
	parser.addOption("c", "Contrast",
		"Contrast adjustment option from. {-100 to 100}",
		CmdLine::OptionType::INT);
	parser.addOption("d", "Demosaic",
		"Demosaicing algorithm selection. {bilinear, hqlinear, freeman, ahd}",
		CmdLine::OptionType::STRING);
	parser.addOption("e", "Exposure",
		"Exposure compensation of the raw data. {-5.0 to 5.0 in EV}",
		CmdLine::OptionType::DOUBLE);
	parser.addOption("i", "Iterations",
		"Demosaicing artefact reduction iterations. {0 to 10}",
		CmdLine::OptionType::INT);
	parser.addOption("T", "Temperature",
		"Color temperature in Kelvin degrees. {2000 to 15000K}",
		CmdLine::OptionType::DOUBLE);
	parser.addOption("t", "Tint",
		"Color balance tint adjustment. {-100 to 100}",
		CmdLine::OptionType::INT);

	// Processing options
	parser.addGroup("Processing options");
	parser.addOption("A", "Name",
		"Artist name for the metadata.", CmdLine::OptionType::STRING);
	parser.addOption("b", "BitDepth",
		"Output file bit depth. {8 or 16}", CmdLine::OptionType::INT);
	parser.addOption("o", "OutputFile",
		"Where to save output. {default: input file name + .tif}",
		CmdLine::OptionType::STRING);
	parser.addOption("p", "profile",
		"Output file color profile. {srgb or argb, default: srgb}",
		CmdLine::OptionType::STRING);
	parser.addSwitch("u", "Don't crop the result. Uncropped.", true);
	parser.addSwitch("x", "Don't RGB process the image. Unprocessed.", true);

	// Other options
	parser.addGroup("Other options");
	parser.addSwitch("h", "Show usage and help text.", false);
	parser.addSwitch("v", "Verbose output.", true);
	parser.addSwitch("V", "Print program version.", false);

	return;
}

/// <summary>
/// Print logo message
/// </summary>
void RawDev::printLogo() const
{
	cout << "=> RawDev <= My Canon raw image developing by hand" << endl;
	cout << "Copyright (C) 2020-2023 by Petr Krajnik" << endl;
	return;
}

/// <summary>
/// Print help and usage message
/// </summary>
void RawDev::printHelp(const CmdLine::Parser &parser) const
{
	printLogo(); // Print logo message
	cout << endl << parser.getUsage("rawdev.exe", "InputFile.cr2");
	return;
}

/// <summary>
/// Print program version
/// </summary>
void RawDev::printVersion() const
{
	printLogo(); // Print logo message
	cout << endl;
	cout << "Version " RAWDEV_VERSION << " (GitHub demonstration version)" << endl;
	cout << "License GPLv3 or later. See LICENSE.md." << endl;
	cout << "Build " << __DATE__ << " " << __TIME__ << endl;
	return;
}

/// <summary>
/// Prints processing summary
/// </summary>
void RawDev::printProcessingSummary(const Image &img) const
{
	cout << "Processing file '"
		<< m_Options.getInputFile().getFileName() << "'"
		<< endl
		<< "  from camera '"
		<< img.getCamProfile()->getCameraModel()
		<< "'" << endl
		<< "  with T=" << m_Options.getTemperature()
		<< "K, tint=" << m_Options.getTint()
		<< ", exposure=" << m_Options.getExposure() << "EV"
		<< ", contrast=" << m_Options.getContrast();
	if (m_Options.getNoCrop())
		cout << ", no crop";
	cout << endl;
	return;
}

/// <summary>
/// Print command line parsing error summary
/// </summary>
/// <param name="errorCount">Error count in arguments</param>
void RawDev::printErrorSummary(int errorCount) const
{
	if (errorCount > 0)
	{
		cerr << endl; // Need newline
		cerr << "They are " << errorCount;
		cerr << " error";
		if (errorCount > 1)
			cerr << 's';
		cerr << " on the command line! EXIT." << endl;
		exit(EXIT_FAILURE);
	}
	return;
}

/// <summary>
/// Load raw image from file
/// </summary>
/// <param name="img">Image storing the file</param>
/// <returns>Result of the loading operation</returns>
bool RawDev::loadRawImage(Image &img)
{
	const Path input = m_Options.getInputFile();
	const double temp = m_Options.getTemperature();

	cout << "Loading raw file from '" << input << "'" << endl;
	StopWatch watch(true);

	try
	{
		img.loadCR2(input, temp);
	}
	catch (const Exception &ex)
	{
		cerr << ex.what() << endl;
		return false;
	}
	watch.stop(); // Stop time measurement
	verbout << "[" << watch << "]" << endl << endl;
	return true;
}
