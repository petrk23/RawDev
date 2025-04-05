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
#include <map>

#include "TiffTag.hpp"
#include "LongTag.hpp"
#include "ShortTag.hpp"
#include "StringTag.hpp"

class TiffDir
{
    typedef std::map<TiffTag::ID, TiffTag*> TagMap;
    TagMap m_tags; // Map of tags for the IFD
    uint32_t m_offset;

public:
    TiffDir(uint32_t offset);
    TiffDir(const TiffDir& tdir);
    TiffDir& operator= (const TiffDir& tdir);
    ~TiffDir();

public: // Public interface
    void clear(void);
    size_t getDirSize() const;
    size_t getExtraSize() const;
    uint32_t getDirOffset() const;

public:
    bool setTag(const TiffTag &tag);
    bool unsetTag(TiffTag::ID id);
    bool getTag(TiffTag::ID id, std::string& val) const;
    bool getTag(TiffTag::ID id, uint32_t& val); // Short and Long
    bool getTag(TiffTag::ID id, std::vector<uint16_t> &data);

public: // Writable and Redable interface
    uint32_t read(std::ifstream& file);
    void write(std::ofstream& file, bool last) const;

private: // IO helpers
    void writeNextDirOffset(
        std::ofstream& file, uint32_t offset, bool last) const;
    void writeTagCount(std::ofstream &file) const;

private:
    void copy(const TiffDir& tdir);
};

////////////////////////////////////////////////////////////////////////////////

inline TiffDir::TiffDir(uint32_t offset)
    : m_tags(), m_offset(offset)
{}

inline TiffDir::TiffDir(const TiffDir& tdir)
{
    copy(tdir);
    return;
}

inline TiffDir& TiffDir::operator= (const TiffDir& tdir)
{
    if (&tdir != this)
    {
        clear();
        copy(tdir);
    }
    return *this;
}

inline TiffDir::~TiffDir()
{
    clear();
    return;
}

inline size_t TiffDir::getDirSize() const
{
    return 12 * m_tags.size() + 6;
}

inline uint32_t TiffDir::getDirOffset() const
{
    return m_offset;
}
