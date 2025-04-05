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

#include "TagFactory.hpp"

#include <memory>
#include <cassert>
using namespace std;

#include "StringTag.hpp"
#include "ShortTag.hpp"
#include "LongTag.hpp"

TiffTag* TagFactory::create(std::ifstream& file, const TiffTag::RawTag& rtag)
{
    TiffTag* res = nullptr;

    // Build tab by type
    switch (rtag.type) {
    case TiffTag::Type::BYTE:
        res = createByteTag(file, rtag);
        break;
    case TiffTag::Type::ASCII:
        res = createStringTag(file, rtag);
        break;
    case TiffTag::Type::SHORT:
        res = createShortTag(file, rtag);
        break;
    case TiffTag::Type::LONG:
        res = createLongTag(file, rtag);
        break;
    case TiffTag::Type::RATIONAL:
        res = createRationaTag(file, rtag);
        break;
    default: // Unsupported type
        break;
    }
    return res;
}

TiffTag* TagFactory::createByteTag(std::ifstream&, const TiffTag::RawTag&)
{
    // Not implemented as it is unused...
    return nullptr;
}

TiffTag* TagFactory::createStringTag(std::ifstream& file, const TiffTag::RawTag& rtag)
{
    TiffTag* res = nullptr;

    if (isValidStringTag(rtag)) {
        if (rtag.count >= 4) {
            file.seekg(rtag.offset, ifstream::beg);
            unique_ptr<char[]> strPtr(new char[rtag.count]);
            file.read(strPtr.get(), rtag.count);
            if (strPtr[rtag.count - 1] != '\0')
                strPtr[rtag.count - 1] = '\0'; // Fix nonterminated string
            res = new StringTag(rtag.id, strPtr.get());
            // res->setOffset(rtag.offset);
        }
        else {// 0-3 chars
            char str[4]; str[rtag.count] = '\0';
            const char* p = reinterpret_cast<const char*>(&rtag.offset);
            for (uint32_t c = 0; c < rtag.count; c++)
                str[c] = p[c]; // Copy chars
            res = new StringTag(rtag.id, str);
        }
    }
    return res;
}

bool TagFactory::isValidStringTag(const TiffTag::RawTag& rtag)
{
    const bool validID = rtag.id == TiffTag::ID::Artist
        || rtag.id == TiffTag::ID::Copyright
        || rtag.id == TiffTag::ID::DateTime
        || rtag.id == TiffTag::ID::DocumentName
        || rtag.id == TiffTag::ID::HostComputer
        || rtag.id == TiffTag::ID::Make
        || rtag.id == TiffTag::ID::Model
        || rtag.id == TiffTag::ID::Software;
    return validID;
}

TiffTag* TagFactory::createShortTag(std::ifstream& file, const TiffTag::RawTag& rtag)
{
    ShortTag* res = nullptr;

    if (isValidShortTag(rtag)) {
        assert(rtag.count > 0);
        if (rtag.count > 2) {
            file.seekg(rtag.offset, ifstream::beg);
            unique_ptr<uint16_t[]> strPtr(new uint16_t[rtag.count]);
            file.read(reinterpret_cast<char*>(strPtr.get()), rtag.count * sizeof(uint16_t));
            vector<uint16_t> data;
            for (uint32_t c = 0; c < rtag.count; c++)
                data.push_back(strPtr[c]);
            res = new ShortTag(rtag.id, data);
            // res->setOffset(rtag.offset);
        }
        else {// 1-2 shorts
            const uint16_t* p = reinterpret_cast<const uint16_t*>(&rtag.offset);
            vector<uint16_t> data;
            for (uint32_t c = 0; c < rtag.count; c++)
                data.push_back(p[c]);
            res = new ShortTag(rtag.id, data);
        }
    }
    return res;
}

bool TagFactory::isValidShortTag(const TiffTag::RawTag& rtag)
{
    bool valid = false;

    switch (rtag.id) {
    // One short element tags
    case TiffTag::ID::ImageLenght:
    case TiffTag::ID::ImageWidth:
    case TiffTag::ID::Compression:
    case TiffTag::ID::Orientation:
    case TiffTag::ID::SamplesPerPixel:
    case TiffTag::ID::PhotometricInterpretation:
    case TiffTag::ID::PlanarConfiguration:
    case TiffTag::ID::ResolutionUnit:
    case TiffTag::ID::RowsPerStrip:
        if (rtag.count == 1)
            valid = true; // Must be one element
        break;
    // Multiple short elements
    case TiffTag::ID::BitsPerSample:
    case TiffTag::ID::StripOffsets:
    case TiffTag::ID::StripByteCounts:
    case TiffTag::ID::CR2slicing:
        valid = true;
        break;
    // Wrong ID for shorts
    default:
        break;
    }
    return valid;
}

TiffTag* TagFactory::createLongTag(std::ifstream&, const TiffTag::RawTag& rtag)
{
    LongTag* res = nullptr;

    if (isValidShortTag(rtag)) {
        assert(rtag.count > 0);
        if (rtag.count > 1) {
            /*file.seekg(rtag.offset, ifstream::beg);
            unique_ptr<uint16_t[]> strPtr(new uint16_t[rtag.count]);
            file.read(reinterpret_cast<char*>(strPtr.get()), rtag.count * sizeof(uint16_t));
            vector<uint16_t> data;
            for (int c = 0; c < rtag.count; c++)
                data.push_back(strPtr[c]);
            res = new ShortTag(rtag.id, data);
            // res->setOffset(rtag.offset);*/
        }
        else {// 1 long
            res = new LongTag(rtag.id, rtag.offset);
        }
    }
    return res;
}

bool TagFactory::isValidLongTag(const TiffTag::RawTag& rtag)
{
    bool valid = false;

    switch (rtag.id) {
        // One short element tags
    case TiffTag::ID::ImageLenght:
    case TiffTag::ID::ImageWidth:
        if (rtag.count == 1)
            valid = true; // Must be one element
        break;
        // Multiple short elements
    case TiffTag::ID::StripOffsets:
    case TiffTag::ID::StripByteCounts:
        valid = true;
        break;
        // Wrong ID for shorts
    default:
        break;
    }
    return valid;
}

TiffTag* TagFactory::createRationaTag(std::ifstream&, const TiffTag::RawTag&)
{
    // Not implemented as it is unused...
    return nullptr;
}
