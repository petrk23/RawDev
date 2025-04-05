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

#include "TiffWriter.hpp"

#include "TiffHeader.hpp"
#include "ByteTag.hpp"
#include "StringTag.hpp"
#include "LongTag.hpp"
#include "ShortTag.hpp"
#include "RationalTag.hpp"

#include "Structures/Array2D.hpp"
#include "Structures/Rect.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "Color.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include "Version.hpp"

// ICC color profiles
#include "ColorProfiles/ColorProfile.hpp"
#include "ColorProfiles/AdobeRGB1998-icc.hpp"
#include "ColorProfiles/srgb-icc.hpp"

#include <ctime>
#include <memory>
using namespace std;

/// <summary>
/// TiffWriter standard constructor
/// </summary>
/// <param name="fileName">Path to the output file</param>
TiffWriter::TiffWriter(const string& fileName, int bits, bool noCrop)
    : m_fileName(fileName), m_file(), m_ifd0(8), m_bits(bits), m_noCrop(noCrop)
{
    assert(bits == 8 || bits == 16);
    if (bits != 8 && bits != 16)
        m_bits = 16;
    constexpr size_t kBUFFLEN = 65536;
    m_writeBuffer = new char[kBUFFLEN];
    m_file.rdbuf()->pubsetbuf(m_writeBuffer, kBUFFLEN);
    return;
}

TiffWriter::~TiffWriter()
{
    if (m_writeBuffer != nullptr)
    {
        delete[] m_writeBuffer;
        m_writeBuffer = nullptr;
    }
    return;
}

/// <summary>
/// Write image into a tiff file
/// </summary>
/// <param name="img">Image to be writen</param>
/// <exception cref="IOException">
/// If IO operation fails it throws IOException with error description.
/// </exception>
void TiffWriter::write(const Image& img)
{
    setupMandatoryTags(img); // Mandatory flags from spec.
    setupOptionalTags(); // Optional tags set
    setDataOffset(); // Calc and set data offset (stripe)

    // Write data into file
    m_file.open(m_fileName, ofstream::binary);
    if (m_file.is_open() == true)
    {
        writeHeader();
        writeIFDs();
        writeData(img);
        m_file.close();
    }
    else throw IOException(TiffWriter::kModuleName, m_fileName,
        "Could not open the output file for writing");
    clearTags(); // Restore begin tag state
    return;
}

/// <summary>
/// Set string tag.
/// </summary>
/// <param name="id">String tag ID</param>
/// <param name="val">String tag value</param>
/// <remarks>
/// Empty string removes the tag.
/// </remarks>
void TiffWriter::setStringTag(TiffTag::ID id, const string& val)
{
    // Add new tag if not empty
    if (val.empty() == false)
        setTag(StringTag(id, val));
    else
        unsetTag(id);
    return;
}

/// <summary>
/// Sets creation date and time tag
/// </summary>
void TiffWriter::setDateTimeTag()
{
    time_t rawtime; time(&rawtime);
    tm timeinfo; Utils::localTime(&rawtime, &timeinfo);
    char timebuff[32];

    // Format to string and set tag
    strftime(timebuff, 32, "%Y:%m:%d %T", &timeinfo);
    setStringTag(TiffTag::ID::DateTime, timebuff);
    return;
}

/// <summary>
/// Adds mandatory tags base on the image data
/// </summary>
/// <param name="img">Source image</param>
void TiffWriter::setupMandatoryTags(const Image& img)
{
    uint16_t width, height, bits;

    if (m_noCrop == false)
    {
        const Rect crop = img.getCamProfile()->getCrop();
        width = static_cast<uint16_t>(crop.getWidth());
        height = static_cast<uint16_t>(crop.getHeight());
    }
    else
    {
        width = static_cast<uint16_t>(img.getWidth());
        height = static_cast<uint16_t>(img.getHeight());
    }

    // Channel info
    bits = static_cast<uint16_t>(m_bits);
    setTag(ShortTag(TiffTag::ID::ImageWidth, width));
    setTag(ShortTag(TiffTag::ID::ImageLenght, height));
    setTag(ShortTag(TiffTag::ID::BitsPerSample, bits, bits, bits));
    setTag(ShortTag(TiffTag::ID::SamplesPerPixel, 3));
    setTag(ShortTag(TiffTag::ID::PhotometricInterpretation, 2));
    setTag(ShortTag(TiffTag::ID::PlanarConfiguration, 1));
    setTag(ShortTag(TiffTag::ID::Orientation, 1));

    // Strip tags
    setTag(LongTag(TiffTag::ID::StripOffsets, 0));
    const uint32_t stripByteCount = static_cast<uint32_t>(3) * width * height * m_bits/8;
    setTag(LongTag(TiffTag::ID::StripByteCounts, stripByteCount));
    setTag(LongTag(TiffTag::ID::RowsPerStrip, height));

    // Resultion 300 DPI measured in inches
    setTag(RationalTag(TiffTag::ID::XResolution, 300, 1));
    setTag(RationalTag(TiffTag::ID::YResolution, 300, 1));
    setTag(ShortTag(TiffTag::ID::ResolutionUnit, 2));

    // Additional mandatory
    setTag(ShortTag(TiffTag::ID::Compression, 1));
    return;
}

/// <summary>
/// Setup optional tags
/// </summary>
void TiffWriter::setupOptionalTags()
{
    setStringTag(TiffTag::ID::Software, "RawDev v" RAWDEV_VERSION);
    // setStringTag(TiffTag::ID::HostComputer, "Windows 10 x64");
    setDateTimeTag(); // Set creation date time tag
    return;
}

/// <summary>
/// Calculates and sets data block offset
/// </summary>
void TiffWriter::setDataOffset()
{
    const uint32_t offset = static_cast<uint32_t>(
        sizeof(TiffHeader) + m_ifd0.getDirSize() + m_ifd0.getExtraSize());
    setTag(LongTag(TiffTag::ID::StripOffsets, offset));
    return;
}

/// <summary>
/// Sets image ICC profile tag
/// </summary>
/// <param name="iccPath">Path to the ICC profile</param>
/// <returns>True on success. False on failure.</returns>
bool TiffWriter::setICC(ColorProfile icc)
{
    if (icc == ColorProfile::aRGB)
        setTag(ByteTag(TiffTag::ID::ICC,
            reinterpret_cast<const char *>(AdobeRGB1998_icc), AdobeRGB1998_icc_len));
    else if (icc == ColorProfile::sRGB)
        setTag(ByteTag(TiffTag::ID::ICC,
            reinterpret_cast<const char *>(srgb_icc), srgb_icc_len));
    else
        return false;
    return true;
}

/// <summary>
/// Write tiff header
/// </summary>
void TiffWriter::writeHeader()
{
    TiffHeader head; // Header of the TIFF file format
    head.firstIFDoffset = sizeof(TiffHeader);
    m_file.write(reinterpret_cast<char*>(&head), sizeof(TiffHeader));
    if (m_file.good() == false)
    {
        throw IOException(TiffWriter::kModuleName, m_fileName,
            "Failed to write tiff header into that file.");
    }
    return;
}

/// <summary>
/// Write image data block to file
/// </summary>
/// <param name="img">Image data to write</param>
void TiffWriter::writeData(const Image& img)
{
    if (m_bits == 16)
    {
        Array2D<Color::RGB16> img16;
        img.convert16(img16, m_noCrop); // Convert image double to 16bit and crop
        const int bytes = img16.getWidth() * img16.getHeight() * sizeof(Color::RGB16);
        m_file.write(reinterpret_cast<char*>(img16[0]), bytes);
    }
    else // m_bits == 8
    {
        assert(m_bits == 8);
        Array2D<Color::RGB8> img8;
        img.convert8(img8, m_noCrop); // Convert image double to 8bit and crop
        const int bytes = img8.getWidth() * img8.getHeight() * sizeof(Color::RGB8);
        m_file.write(reinterpret_cast<char*>(img8[0]), bytes);
    }

    // Check the results
    if (m_file.good() == false)
    {
        throw IOException(TiffWriter::kModuleName, m_fileName,
            "Failed to write tiff data into that file.");
    }
    return;
}
