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

#include <string>

#include "ColorProfiles/ColorProfile.hpp"
#include "Demosaic/AlgorithmType.hpp"
#include "Structures/Path.hpp"
namespace CmdLine {
class Parser;
};

class Options {
    // I/O files
    Path m_InputFile;
    Path m_OutputFile;

    // Processing options
    int m_Tint, m_Contrast, m_DemosaicIter;
    double m_Temperature, m_Exposure;
    bool m_NoCrop, m_NoProcess, m_Verbose;
    Demosaic::AlgorithmType m_DemosaicAlg;
    int m_bitDepth;
    ColorProfile m_colorProfile;

    // Metadata
    std::string m_Artist;

public:
    Options();
    ~Options() = default;
    int process(const CmdLine::Parser& parser);

public: // Geters of options
    Path getInputFile() const;
    Path getOutputFile() const;
    bool getNoCrop() const;
    bool getNoProcess() const;
    bool getVerbose() const;
    int getTint() const;
    int getContrast() const;
    int getDemosaicIter() const;
    double getTemperature() const;
    double getExposure() const;
    Demosaic::AlgorithmType getDemosaicAlg() const;
    int getBitDepth() const;
    ColorProfile getColorProfile() const;
    std::string getArtistName() const;

private: // Helpers
    int processInputFile(const CmdLine::Parser& parser);
    [[nodiscard]] bool notCanonRawExtension(const std::string& ext);
    int processOutputFile(const CmdLine::Parser& parser);
    int processTint(const CmdLine::Parser& parser);
    int processContrast(const CmdLine::Parser& parser);
    int processDemosaicIter(const CmdLine::Parser& parser);
    int processTemperature(const CmdLine::Parser& parser);
    int processExposure(const CmdLine::Parser& parser);
    int processDemosaicAlg(const CmdLine::Parser& parser);
    int processBitDepth(const CmdLine::Parser& parser);
    int processColorProfile(const CmdLine::Parser& parser);
    int processArtistName(const CmdLine::Parser& parser);
};

////////////////////////////////////////////////////////////////////////////////

inline Options::Options()
    : m_InputFile(), m_OutputFile("output.tif"),
      m_Tint(0),
      m_Contrast(25),
      m_DemosaicIter(3),
      m_Temperature(5000.0),
      m_Exposure(0.0),
      m_NoCrop(false),
      m_NoProcess(false),
      m_Verbose(false),
      m_DemosaicAlg(Demosaic::AlgorithmType::AHD),
      m_bitDepth(8),
      m_colorProfile(ColorProfile::sRGB),
      m_Artist()
{
}

inline Path Options::getInputFile() const
{
    return m_InputFile;
}

inline Path Options::getOutputFile() const
{
    return m_OutputFile;
}

inline bool Options::getNoCrop() const
{
    return m_NoCrop;
}

inline bool Options::getNoProcess() const
{
    return m_NoProcess;
}

inline bool Options::getVerbose() const
{
    return m_Verbose;
}

inline int Options::getTint() const
{
    return m_Tint;
}

inline int Options::getContrast() const
{
    return m_Contrast;
}

inline int Options::getDemosaicIter() const
{
    return m_DemosaicIter;
}

inline double Options::getTemperature() const
{
    return m_Temperature;
}

inline double Options::getExposure() const
{
    return m_Exposure;
}

inline Demosaic::AlgorithmType Options::getDemosaicAlg() const
{
    return m_DemosaicAlg;
}

inline int Options::getBitDepth() const
{
    return m_bitDepth;
}

inline ColorProfile Options::getColorProfile() const
{
    return m_colorProfile;
}

inline std::string Options::getArtistName() const
{
    return m_Artist;
}

inline bool Options::notCanonRawExtension(const std::string& ext)
{
    return  ext != "CR2" && ext != "cr2";
}
