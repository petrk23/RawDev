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

#include <fstream>
#include <string>

#include "TiffDir.hpp"
#include "TiffTag.hpp"

#include "NonCopyable.hpp"
#include "Structures/Image.hpp"
enum ColorProfile : int;

class TiffWriter : NonCopyable
{
    static constexpr const char* kModuleName = "TiffWriter";

    std::string m_fileName;
    std::ofstream m_file;
    TiffDir m_ifd0; // IFD0
    int m_bits;
    bool m_noCrop;
    char* m_writeBuffer;

public:
    TiffWriter(const std::string& fileName, int bits, bool noCrop);
    ~TiffWriter();

public: // Writer interface
    void write(const Image& img);

public: // Additional tag set
    void setDocumentName(const std::string& docname);
    void setMake(const std::string& make);
    void setModel(const std::string& model);
    void setArtist(const std::string& artist);
    void setCopyright(const std::string& copyright);
    bool setICC(ColorProfile profile);

private: // Tag manipulation
    void setTag(const TiffTag& tag);
    void unsetTag(TiffTag::ID id);
    void setStringTag(TiffTag::ID id, const std::string& val);
    void setDateTimeTag();
    void setupMandatoryTags(const Image& img);
    void setupOptionalTags();
    void setDataOffset();
    void clearTags(); // Clears tag list

private: // Image write helpers
    void writeHeader();
    void writeIFDs();
    void writeData(const Image& img);
};

////////////////////////////////////////////////////////////////////////////////

inline void TiffWriter::setTag(const TiffTag& tag)
{
    m_ifd0.setTag(tag);
    return;
}

inline void TiffWriter::unsetTag(TiffTag::ID id)
{
    m_ifd0.unsetTag(id);
    return;
}

inline void TiffWriter::setDocumentName(const std::string& docname)
{
    setStringTag(TiffTag::ID::DocumentName, docname);
    return;
}

inline void TiffWriter::setMake(const std::string& make)
{
    setStringTag(TiffTag::ID::Make, make);
    return;
}

inline void TiffWriter::setModel(const std::string& model)
{
    setStringTag(TiffTag::ID::Model, model);
    return;
}

inline void TiffWriter::setArtist(const std::string& artist)
{
    setStringTag(TiffTag::ID::Artist, artist);
    return;
}

inline void TiffWriter::setCopyright(const std::string& copyright)
{
    setStringTag(TiffTag::ID::Copyright, copyright);
    return;
}

inline void TiffWriter::clearTags()
{
    m_ifd0.clear();
    return;
}

inline void TiffWriter::writeIFDs()
{
    m_ifd0.write(m_file, true);
    return;
}
