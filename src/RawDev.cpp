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

#include "RawDev.hpp"

#include <cstdlib>
#include <format>
#include <iostream>

#include "CamProfiles/CamProfile.hpp"
#include "CmdLineParser.hpp"
#include "Exception.hpp"
#include "Options.hpp"
#include "StopWatch.hpp"
#include "Structures/Image.hpp"
#include "Version.hpp"

#include "Demosaic.hpp"
#include "Output.hpp"
#include "ProcRGB.hpp"
#include "Scale.hpp"

using std::cout, std::cerr, std::endl;

/// <summary>
/// Verbose output logger.
/// </summary>
Logger RawDev::verbout(cout);

/// <summary>
/// Run RawDev applicaiton
/// </summary>
/// <param name="argc">Command line argument count</param>
/// <param name="argv">Command line arguments</param>
/// <returns>Application run result code</returns>
int RawDev::run(int argc, const char* argv[])
{
    StopWatch watch(true);

    processCmdLine(argc, argv); // Process cmd line options
    verbout.setEnabled(m_options.getVerbose());

    Image img; // Raw image for processing
    if (!loadRawImage(img)) {
        cerr << "Failed to read input RAW file. EXIT." << endl;
        return EXIT_FAILURE;
    }
    printProcessingSummary(img); // Values of processing options

    verbout.indent(); // Go to itemize mode
    verbout << endl;

    verbout << "Scaling colors in camera native space" << endl;
    verbout.indent();
    ScaleModule::run(img, m_options);
    verbout.unindent();

    verbout << "Demosaicing pixels on the bayer mask" << endl;
    verbout.indent();
    DemosaicModule::run(img, m_options);
    verbout.unindent();

    verbout << "Processing RGB image" << endl;
    verbout.indent();
    ProcRGBModule::run(img, m_options);
    verbout.unindent();

    verbout << "Finishing and output" << endl;
    verbout.indent();
    OutputModule::run(img, m_options);
    verbout.unindent();

    watch.stop(); // Stop measurement
    verbout << endl;
    cout << "DONE in " << watch << "." << endl;

    return EXIT_SUCCESS;
}

/// <summary>
/// Load raw image from file
/// </summary>
/// <param name="img">Image storing the file</param>
/// <returns>Result of the loading operation</returns>
bool RawDev::loadRawImage(Image& img)
{
    const Path input = m_options.getInputFile();
    const double temp = m_options.getTemperature();

    cout << "Loading raw file from '" << input << "'" << endl;
    StopWatch watch(true);

    try {
        img.loadCR2(input, temp);
    }
    catch (const Exception& ex) {
        cerr << ex.what() << endl;
        return false;
    }
    watch.stop(); // Stop time measurement
    verbout << "File loaded in " << watch << '\n' << endl;
    return true;
}

/// <summary>
/// Prints processing summary
/// </summary>
void RawDev::printProcessingSummary(const Image& img)
{
    cout << std::format(
        "Processing file '{}'\n"
        "  from camera '{}'\n"
        "  with T={}K, tint={}, exposure={}EV, contrast={}",
        m_options.getInputFile().getFileName(),
        img.getCamProfile()->getCameraName(),
        m_options.getTemperature(),
        m_options.getTint(),
        m_options.getExposure(),
        m_options.getContrast());

    if (m_options.getNoCrop()) {
        cout << ", no crop";
    }
    cout << endl;
}

/// <summary>
/// Process command line options
/// </summary>
/// <param name="argc">Argument count</param>
/// <param name="argv">Argument values</param>
void RawDev::processCmdLine(int argc, const char* argv[])
{
    CmdLine::Parser parser;
    SetupOptions(parser); // Setup availiable options

    // Parse the command line
    int errorCount = parser.parse(argc, argv);
    if (errorCount == 0) {
        if (parser.foundSwitch("h")) {
            PrintHelp(parser);
            exit(EXIT_SUCCESS);
        }
        else if (parser.foundSwitch("V")) {
            PrintVersion();
            exit(EXIT_SUCCESS);
        }
        else {
            errorCount = m_options.process(parser);
        }
    }
    PrintErrorSummary(errorCount);
}

/// <summary>
/// Setup app availiable options
/// </summary>
/// <param name="parser">Command line argument parser</param>
void RawDev::SetupOptions(CmdLine::Parser& parser)
{
    // Development options
    parser.addGroup("Develop options");
    parser.addOption("c", "Contrast",
        "Contrast adjustment option from. {-100 to 100}",
        CmdLine::OptionType::INT);
    parser.addOption("d", "Demosaic",
        "Demosaicing algortithm selection. {bilinear, hqlinear, freeman, ahd}",
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
        "Color balance tint ajustment. {-100 to 100}",
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
    parser.addSwitch("u", "Don't crop the result. Uncroped.", true);
    parser.addSwitch("x", "Don't RGB process the image. Unprocessed.", true);

    // Other options
    parser.addGroup("Other options");
    parser.addSwitch("h", "Show usage and help text.", false);
    parser.addSwitch("v", "Verbose output.", true);
    parser.addSwitch("V", "Print program version.", false);
}

/// <summary>
/// Print help and usage message
/// </summary>
void RawDev::PrintHelp(const CmdLine::Parser& parser)
{
    PrintLogo(); // Print logo message
    cout << endl // Flush header and print help
         << parser.getUsage("RawDev.exe", "InputFile.cr2");
}

/// <summary>
/// Print program version
/// </summary>
void RawDev::PrintVersion()
{
    PrintLogo(); // Print logo message
    cout << std::format("Version {}, build {} {}\n",
        RAWDEV_VERSION, k_compileDate, __TIME__);
    cout << "License GPLv3 or later. See project 'LICENSE' file.\n"
            "Repository: https://github.com/petrk23/RawDev" << endl;
}

/// <summary>
/// Print logo message
/// </summary>
void RawDev::PrintLogo()
{
    cout << std::format(
        "RawDev: Digital camera RAW image developer\n"
        "Copyright (C) 2020-{} by Petr Krajnik\n",
        k_compileYear);
}

/// <summary>
/// Print command line parsing error summary
/// </summary>
/// <param name="errorCount">Error count in arguments</param>
void RawDev::PrintErrorSummary(int errorCount)
{
    if (errorCount > 0) {
        if (errorCount > 1) {
            cerr << "There are " << errorCount << " errors";
        }
        else {
            assert(errorCount == 1);
            cerr << "There is a error";
        }
        cerr << " on the command line! EXIT." << endl;
        exit(EXIT_FAILURE);
    }
}
