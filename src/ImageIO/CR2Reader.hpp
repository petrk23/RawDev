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

#include <memory>
#include <fstream>
#include <string>
#include <vector>

#include "NonCopyable.hpp"
#include "Structures/Array2D.hpp"
#include "TiffDir.hpp"

struct TiffHeader;
struct RawHeader;
class HuffTree;

namespace RawDevTest { class CR2ReaderTest; };

class CR2Reader : NonCopyable
{
    static constexpr const char* kModuleName = "CR2Reader";
    static constexpr int kReadBufferLen = 65536; // 64kB read buffer
    static const uint8_t kBitMasks[];

    // File attributes
    std::ifstream m_file;
    std::string m_fileName;
    std::unique_ptr<char[]> m_readBuffer;

    // Tiff directory structure
    std::vector<TiffDir> m_dirs;
    int m_actDir;

public:
    CR2Reader(const std::string& fileName);
    friend class RawDevTest::CR2ReaderTest;

public: // Public interface
    void open();
    void read(Array2D<uint16_t>& img);
    void close();
    bool getModel(std::string &model) const;

private: // RAW format reading
    struct DHTHeader;
    struct SOF3Header;
    struct SOSHeader;

    void readTiffHeader(TiffHeader& th);
    void readRawHeader(RawHeader& cr2h);
    void readIFDstructure(const TiffHeader& th);
    void checkIntegrity(RawHeader& cr2h) const;
    void seekToImageData();
    void readDHTheader(DHTHeader& hdr);
    int makeHuffmanTree(DHTHeader& hdr, char* base);
    void readSOF3header(SOF3Header& hdr);
    void readSOSheader(SOSHeader& hdr, const SOF3Header& sof3);
    void loadSlicingInfo(int (&slices)[3]);
    void modelCorrect(int width, int &height, int(&slices)[3]);
    void readMarker(const char* name, unsigned char code);

private: // Raw image data reading
    int m_bitbuff, m_availBits;

    void readRawImage(Array2D<uint16_t>& img, const int(&slices)[3],
        const DHTHeader& dht, const SOF3Header& sof3, const SOSHeader& sos);
    void decode(Array2D<uint16_t> &dimg,
        const DHTHeader &dht, const SOF3Header &sof3, const SOSHeader &sos);
    void decode4(Array2D<uint16_t>& dimg,
        const DHTHeader& dht, const SOF3Header& sof3, const SOSHeader& sos);
    void unslice(Array2D<uint16_t> &img,
        const Array2D<uint16_t> &ximg, const int(&slices)[3]);
    int decodeDiffValue(const HuffTree& htree);
    int getbit();
    int getbits(int bits);
    void loadNextByte();
};

////////////////////////////////////////////////////////////////////////////////

inline CR2Reader::CR2Reader(const std::string& fileName)
    : m_file(), m_fileName(fileName)   // File
    , m_dirs(), m_actDir(-1)           // Tiff structures
    , m_bitbuff(0), m_availBits(0)     // Bit reader
{
    m_readBuffer = std::make_unique<char[]>(CR2Reader::kReadBufferLen);
    return;
}

inline void CR2Reader::close()
{
    m_file.close();
    m_dirs.clear(); m_actDir = -1;
    m_bitbuff = 0;
    m_availBits = 0;
    return;
}

inline bool CR2Reader::getModel(std::string &model) const
{
    if (m_dirs.size() <= 0)
        return false;
    return m_dirs[0].getTag(TiffTag::ID::Model, model);
}
