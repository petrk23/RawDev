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

#include <cinttypes>
#include <fstream>
#include <vector>

struct TiffTag
{
    enum class ID : uint16_t
    {
        ImageWidth = 256,
        ImageLenght = 257,
        BitsPerSample = 258,
        Compression = 259,
        PhotometricInterpretation = 262,
        DocumentName = 269,
        Make = 271,
        Model = 272,
        StripOffsets = 273,
        Orientation = 274,
        SamplesPerPixel = 277,
        RowsPerStrip = 278,
        StripByteCounts = 279,
        XResolution = 282,
        YResolution = 283,
        PlanarConfiguration = 284,
        ResolutionUnit = 296,
        Software = 305,
        DateTime = 306,
        Artist = 315,
        HostComputer = 316,
        Copyright = 33432,
        ICC = 34675,
        CR2slicing = 0xc640
    };

    enum class Type : uint16_t
    {
        BYTE = 1, ASCII, SHORT, LONG, RATIONAL,
        SBYTE, UNDEFINDED, SSHORT, SLONG, SRATIONAL, FLOAT, DOUBLE
    };

    struct RawTag
    {
        TiffTag::ID id;
        TiffTag::Type type;
        uint32_t count;
        uint32_t offset;
    };

protected:
    union { char mem[12]; RawTag val; } m_tag;

public:
    TiffTag(TiffTag::ID id,
        TiffTag::Type type, uint32_t count, uint32_t offset);
    virtual ~TiffTag() {};

public: // Interface
    virtual void write(std::ofstream& file,
        uint32_t& offset, std::vector<char>& extraBytes) = 0;
    virtual size_t extra() const = 0;
    virtual TiffTag* clone() const = 0;

public: // Getters
    TiffTag::ID getID() const;

protected: // Helpers
    void writeTag(std::ofstream& file) const;
};

////////////////////////////////////////////////////////////////////////////////

inline TiffTag::TiffTag(TiffTag::ID id,
    TiffTag::Type type, uint32_t count, uint32_t offset)
{
    m_tag.val.id = id;
    m_tag.val.type = type;
    m_tag.val.count = count;
    m_tag.val.offset = offset;
    return;
}

inline TiffTag::ID TiffTag::getID() const
{
    return m_tag.val.id;
}

inline void TiffTag::writeTag(std::ofstream& file) const
{
    file.write(m_tag.mem, sizeof(RawTag));
    return;
}
