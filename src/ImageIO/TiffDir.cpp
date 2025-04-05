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

#include "TiffDir.hpp"

#include <memory>
#include <vector>
using namespace std;

#include "TagFactory.hpp"
#include "Utils.hpp"

/*
Read directory from file
*/
uint32_t TiffDir::read(ifstream& file)
{
    // Read tag count
    uint16_t tagCount;
    file.read(reinterpret_cast<char*>(&tagCount), sizeof(tagCount));

    // Read directory
    const int size = 12 * tagCount + 4;
    unique_ptr<char[]> dirmem(new char[size]);
    file.read(dirmem.get(), size);
    TiffTag::RawTag* rawTagPtr =
        reinterpret_cast<TiffTag::RawTag*>(dirmem.get());
    TagFactory tagFactory;

    // Read tags
    for (int c = 0; c < tagCount; c++)
    {
        TiffTag* t = tagFactory.create(file, rawTagPtr[c]);
        if (t != nullptr)
        {
            setTag(*t);
            delete t;
        }
    }
    // Read next IFD
    uint32_t* q = reinterpret_cast<uint32_t*>(dirmem.get() + size - 4);
    return *q;
}

/*
Write directory into a file
*/
void TiffDir::write(ofstream& file, bool last) const
{
    uint32_t dataBaseOffset = static_cast<uint32_t>(
        static_cast<size_t>(file.tellp()) + getDirSize());
    vector<char> extraBytes; // Extra IFD data section

    // Write directory
    writeTagCount(file);
    for (auto& mtag : m_tags)
        mtag.second->write(file, dataBaseOffset, extraBytes);
    writeNextDirOffset(file, dataBaseOffset, last);

    // Write directory extra data
    file.write(extraBytes.data(), extraBytes.size());
    return;
}

/*
Set tag to the directory
*/
bool TiffDir::setTag(const TiffTag& tag)
{
    const TiffTag::ID id = tag.getID();
    const bool res = unsetTag(id); // Remove old if present
    m_tags[id] = tag.clone();
    return res;
}

/*
Unset tag to the directory
*/
bool TiffDir::unsetTag(TiffTag::ID id)
{
    TagMap::iterator tag = m_tags.find(id);
    if (tag != m_tags.end())
    {
        delete tag->second;
        m_tags.erase(tag);
        return true;
    }
    return false;
}

/*
Clear the tags in that directory
*/
void TiffDir::clear(void)
{
    for (auto& mtag : m_tags)
        delete mtag.second;
    m_tags.clear();
    return;
}

/*
Get extra size of that directory
*/
size_t TiffDir::getExtraSize() const
{
    size_t extra = 0;
    for (auto& mtag : m_tags)
        extra += mtag.second->extra();
    return extra;
}

/*
Write directory tag count
*/
void TiffDir::writeTagCount(ofstream& file) const
{
    const uint16_t count = static_cast<uint16_t>(m_tags.size());
    file.write(reinterpret_cast<const char*>(&count), 2);
    return;
}

/*
Write next directory offset link
*/
void TiffDir::writeNextDirOffset(
    ofstream& file, uint32_t offset, bool last) const
{
    if (last == false)
        file.write(reinterpret_cast<const char*>(&offset), 4);
    else file.write("\0\0\0\0", 4); // Last IDF
    return;
}

/*
Fill directory with deep copy
*/
void TiffDir::copy(const TiffDir& tdir)
{
    m_offset = tdir.m_offset;
    for (auto& mtag : tdir.m_tags)
        m_tags[mtag.second->getID()] = mtag.second->clone();
    return;
}

/*
Get string tag
*/
bool TiffDir::getTag(TiffTag::ID id, std::string& val) const
{
    TagMap::const_iterator it = m_tags.find(id);
    if (it != m_tags.end())
    {
        auto tag = dynamic_cast<StringTag*>(it->second);
        if (tag != nullptr)
        {
            StringTag* p = reinterpret_cast<StringTag*>(it->second);
            p->getVal(val);
        }
        return true;
    }
    return false;
}

/*
Get 32bit uint tag
*/
bool TiffDir::getTag(TiffTag::ID id, uint32_t& val)
{
    TagMap::iterator it = m_tags.find(id);
    if (it != m_tags.end())
    {
        auto tag = dynamic_cast<LongTag*>(it->second);
        if (tag != nullptr)
        {
            LongTag* p = reinterpret_cast<LongTag*>(it->second);
            val = p->getVal();
        }
        return true;
    }
    return false;
}

/*
Get 16bit uint tag.
*/
bool TiffDir::getTag(TiffTag::ID id, std::vector<uint16_t>& data)
{
    TagMap::iterator it = m_tags.find(id);
    if (it != m_tags.end())
    {
        auto tag = dynamic_cast<ShortTag*>(it->second);
        if (tag != nullptr)
        {
            ShortTag* p = reinterpret_cast<ShortTag*>(it->second);
            p->getVal(data);
        }
        return true;
    }
    return false;
}
