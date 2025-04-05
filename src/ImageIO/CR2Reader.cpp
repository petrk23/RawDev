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

#include "CR2Reader.hpp"

#include "TiffHeader.hpp"
#include "RawHeader.hpp"
#include "HuffTree.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

using namespace std;

/// <summary>
/// Define Huffman Tables structure
/// </summary>
struct CR2Reader::DHTHeader {
    HuffTree huff[4];
};

/// <summary>
/// Start of frame 3 header structure
/// </summary>
/// <remarks>
/// Needed for bit dept, components and sampling freq.
/// </remarks>
struct CR2Reader::SOF3Header {
    uint16_t lines, samples;
    uint8_t samplePrec, components;
};

/// <summary>
/// Start of scan header structure
/// </summary>
/// <remarks>
/// Needed for components and huffman tree selection
/// </remarks>
struct CR2Reader::SOSHeader {
    uint8_t components, tableSel[4][2]; // [Channel]->DC|AC
};

/// <summary>
/// Open image for read
/// </summary>
void CR2Reader::open()
{
    m_file.open(m_fileName, ifstream::binary);
    if (m_file.is_open() == true) {
        // Setup the read buffer
        m_file.rdbuf()->pubsetbuf(// Set file reader buffer
            m_readBuffer.get(), CR2Reader::kReadBufferLen);

        // Read raw format TIFF structure
        TiffHeader th; readTiffHeader(th);
        RawHeader cr2; readRawHeader(cr2);
        readIFDstructure(th); // Reads the IFD structure
        checkIntegrity(cr2);  // Links, tag len, ...
    }
    else throw IOException(CR2Reader::kModuleName,
        m_fileName, "Input raw file could not be opened.");
    return;
}

/// <summary>
/// Reading base tiff header
/// </summary>
/// <param name="th">Tiff header structure</param>
void CR2Reader::readTiffHeader(TiffHeader& th)
{
    char header[8]; // Header buffer
    m_file.read(header, sizeof(header));
    if (m_file.good() == false)
    {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Failed to read the TIFF header.");
    }

    // Fill the structure
    th.byteOrder[0] = header[0];
    th.byteOrder[1] = header[1];
    th.magic = *(reinterpret_cast<uint16_t*>(header + 2));
    th.firstIFDoffset = *(reinterpret_cast<uint32_t*>(header + 4));

    // Read was success so check the data
    const bool dataOK = th.byteOrder[0] == 'I'
        && th.byteOrder[1] == 'I' && th.magic == 42;
    if (dataOK == false)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Wrong or usupported data in the TIFF header.");
    }
    if (th.firstIFDoffset < 16) {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Wrong first IFD offset. Maybe its a tiff and not CR2.");
    }
    return;
}

/// <summary>
/// Read CR2 raw header
/// </summary>
/// <param name="cr2h">Raw header structure</param>
void CR2Reader::readRawHeader(RawHeader& cr2h)
{
    char header[8];
    m_file.read(header, sizeof(header));
    if (m_file.good() == false)
    {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Failed to read the CR2 header.");
    }

    // Fill the structure
    cr2h.rawMarker[0] = header[0];
    cr2h.rawMarker[1] = header[1];
    cr2h.version = *(reinterpret_cast<uint16_t*>(header + 2));
    cr2h.rawIFDoffset = *(reinterpret_cast<uint32_t*>(header + 4));

    // Read was success so check the data
    const bool dataOK = cr2h.rawMarker[0] == 'C'
        && cr2h.rawMarker[1] == 'R' && cr2h.version == 2
        && cr2h.rawIFDoffset >= 16; // TiffHead + RawHead = 16B
    if (dataOK == false)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Wrong data in the CR2 raw header.");
    }
    return;
}

/// <summary>
/// Read the raw (TIFF) IFD structure
/// </summary>
/// <param name="th">Tiff header structure</param>
void CR2Reader::readIFDstructure(const TiffHeader& th)
{
    uint32_t offset = th.firstIFDoffset;
    while (offset != 0)
    {
        m_file.seekg(offset, ifstream::beg);
        if (m_file.good() == false)
        {
            throw IOException(CR2Reader::kModuleName, m_fileName,
                "Seek to the next IFD failed. Maybe wrong offset value.");
        }
        m_dirs.push_back(TiffDir(offset));
        m_actDir++; // Set next directory
        offset = m_dirs[m_actDir].read(m_file);
    }
    return;
}

/// <summary>
/// Check integrity of the read raw image structure
/// </summary>
/// <param name="cr2h"></param>
void CR2Reader::checkIntegrity(RawHeader& cr2h) const
{
    // Check directory count
    const size_t dirCount = m_dirs.size();
    if (dirCount != 4 && dirCount != 5)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "They are not three directories in file.");
    }

    // Check raw IFD offset
    if (m_dirs[3].getDirOffset() != cr2h.rawIFDoffset)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Mismatch of raw header and real data offset.");
    }
    return;
}

/// <summary>
/// Read RAW image data from the file
/// </summary>
/// <param name="img"></param>
void CR2Reader::read(Array2D<uint16_t>& img)
{
    // Seek to the image data begin
    seekToImageData();

    // Read the structures (Loosless JPEG)
    // Fixed oder of headers. More strict than the standard!
    readMarker("StartOfImage", 0xd8);
    DHTHeader dht; // Define Huffmann Tables
    readDHTheader(dht);
    SOF3Header sof3; // Start of Frame 3 Header
    readSOF3header(sof3);
    SOSHeader sos; // Start of Scan Header
    readSOSheader(sos, sof3);

    int slices[3]; // Slice info
    loadSlicingInfo(slices);

    // Image data
    int height = sof3.lines;
    const int width = slices[0] * slices[1] + slices[2];
    modelCorrect(width, height, slices); // Model correctures
    img = Array2D<uint16_t>(width, height); // Allocate new image

    // Read RAW image data
    readRawImage(img, slices, dht, sof3, sos);
    readMarker("EndOfImage", 0xd9); // Match end of image marker
    return;
}

/// <summary>
/// Seek to the start of the image data start
/// </summary>
void CR2Reader::seekToImageData()
{
    // If the file is closed
    if (m_file.is_open() == false)
    {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Can't read data as the file is not open.");
    }

    // Get strip offset from tag
    uint32_t stripOffset;
    m_dirs[3].getTag(TiffTag::ID::StripOffsets, stripOffset);

    // Goto the image data start
    m_file.seekg(stripOffset, ifstream::beg);
    if (m_file.good() == false)
    {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Seek to the image data section failed");
    }
    return;
}

/// <summary>
/// Read and match marker
/// </summary>
void CR2Reader::readMarker(const char* name, unsigned char code)
{
    unsigned char marker[2];
    m_file.read(reinterpret_cast<char*>(marker), 2);
    if (m_file.good() == false)
    {
        stringstream ss;
        ss << "Failed to read '" << name << "' marker.";
        throw IOException(CR2Reader::kModuleName, m_fileName, ss.str());
    }
    else if (marker[0] != 0xff || marker[1] != code)
    {
        stringstream ss;
        ss << "Marker '" << name << "' not found";
        throw FormatException(CR2Reader::kModuleName, m_fileName, ss.str());
    }
    return;
}

/// <summary>
/// Read define huffman table header
/// </summary>
/// <param name="hdr">Header structure to store the data</param>
void CR2Reader::readDHTheader(DHTHeader& hdr)
{
    // Read marker
    readMarker("DefineHuffmanTable", 0xc4);

    // Read len of DHT
    uint16_t len; // Lenght of the header
    m_file.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (m_file.good() == false)
    {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Failed to read DHT header lenght.");
    }
    len = Utils::byteSwapU16(len) - 2;

    // Check the len of DHT
    if (len < 18)
    {// Minimal size of DHT
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "DHT header lenght too small.");
    }

    // Load header
    unique_ptr<char[]> header = make_unique<char[]>(len);
    m_file.read(header.get(), len);
    if (m_file.good() == false)
    {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Failed to read DHT data.");
    }

    // Make Huffman trees
    int k = 0;
    while (k < len)
        k += 17 + makeHuffmanTree(hdr, header.get() + k);
    if (k > len)
    {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Decoded DHT lenght doesnt match the given lenght.");
    }
    return;
}

/// <summary>
/// Make huffman tree from the data read from file
/// </summary>
/// <param name="hdr">Define huffman table header</param>
/// <param name="base">Base pointer to the read data</param>
int CR2Reader::makeHuffmanTree(DHTHeader& hdr, char* base)
{
    // Class / Index load
    uint8_t* hinfo = reinterpret_cast<uint8_t*>(base + 1);
    const uint8_t tindex = hinfo[-1] & 0x0f; // Table index
    if ((hinfo[-1] >> 4) != 0 || tindex > 3)
    {
        // All tables must be DC=0 for loosless
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Wrong huffman table class info.");
    }

    // Check huffman table
    int i = 0, keywordCount = 0;
    while (i < 16 && hinfo[i] == 0)
        i++;
    while (i < 16 && hinfo[i] != 0) {
        keywordCount += hinfo[i]; i++;
    }
    while (i < 16 && hinfo[i] == 0)
        i++;
    if (i != 16 || hinfo[keywordCount] != 0 || hinfo[0] != 0)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Corrupted huffman table detected.");
    }
    hdr.huff[tindex](hinfo, hinfo + 16); // Build huffman tree
    return keywordCount;
}

/// <summary>
/// Read start of frame 3 header
/// </summary>
/// <param name="hdr">Header structure to store the data</param>
void CR2Reader::readSOF3header(SOF3Header& hdr)
{
    readMarker("StartOfFrame3", 0xc3);

    uint16_t len; // Read header len
    m_file.read(reinterpret_cast<char*>(&len), sizeof(len));
    len = Utils::byteSwapU16(len);
    if (len != 14 && len != 20)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Wrong SOF3 header lenght value.");
    }

    // Load header
    unique_ptr<char[]> header = make_unique<char[]>(len);
    m_file.read(header.get(), len - 2);
    if (m_file.good() == false) {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Failed to read SOF3 header data.");
    }

    // Fill header structure
    hdr.samplePrec = *(reinterpret_cast<uint8_t*>(header.get()));
    hdr.lines = *(reinterpret_cast<uint16_t*>(header.get() + 1));
    hdr.lines = Utils::byteSwapU16(hdr.lines);
    hdr.samples = *(reinterpret_cast<uint16_t*>(header.get() + 3));
    hdr.samples = Utils::byteSwapU16(hdr.samples);
    hdr.components = *(reinterpret_cast<uint8_t*>(header.get() + 5));

    // Check read data
    if ((hdr.samplePrec != 14 && hdr.samplePrec != 12)
        || (hdr.components != 4 && hdr.components != 2))
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Wrong SOF3 data or not full raw (eg. sraw or mraw).");
    }

    // Read (unity tables)
    uint8_t* p = reinterpret_cast<uint8_t*>(header.get()) + 6;
    for (size_t c = 0; c < hdr.components; c++, p += 3)
    {
        const bool dataOK = p[0] == c + 1 && ((p[1] & 0xf0) >> 4) == 1
            && (p[1] & 0xf) == 1 && p[2] == 0;
        if (dataOK == false)
        {
            throw FormatException(CR2Reader::kModuleName,
                m_fileName, "Wrong SOF3 sampling table data.");
        }
    }
    return;
}

/// <summary>
/// Read star of scan header
/// </summary>
/// <param name="hdr">Header structure for data storage</param>
void CR2Reader::readSOSheader(SOSHeader& hdr, const SOF3Header& sof3)
{
    readMarker("StartOfScan", 0xda);

    uint16_t len; // Read SOS header len
    m_file.read(reinterpret_cast<char*>(&len), sizeof(len));
    len = Utils::byteSwapU16(len);
    if (len != 14 && len != 10)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Wrong SOS header lenght.");
    }

    // Read data
    unique_ptr<char[]> header = make_unique<char[]>(len);
    m_file.read(header.get(), len - 2);
    if (m_file.good() == false) {
        throw IOException(CR2Reader::kModuleName,
            m_fileName, "Failed to read SOS header data.");
    }

    // Fill the structure
    uint8_t* p = reinterpret_cast<uint8_t*>(header.get());
    hdr.components = p[0]; p++; // Extract components
    if (hdr.components != sof3.components)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Component count mismatch between headers.");
    }

    // Table of selectors
    for (int c = 0; c < hdr.components; c++, p += 2) {
        hdr.tableSel[c][0] = (p[1] & 0xf0) >> 4;
        hdr.tableSel[c][1] = p[1] & 0xf;
    }

    // Read constants (1|0|0)
    if (p[0] != 1 || p[1] != 0 || p[2] != 0)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Last SOS constant data dont match.");
    }
    return;
}

/// <summary>
/// Load structure about raw image slicing
/// </summary>
/// <param name="slices">Output slicing array</param>
void CR2Reader::loadSlicingInfo(int(&slices)[3])
{
    // Load info from tag
    vector<uint16_t> data; // Get slice tag data
    m_dirs[3].getTag(TiffTag::ID::CR2slicing, data);
    slices[0] = data[0];
    slices[1] = data[1];
    slices[2] = data[2];

    // Check information
    // 2020/06/12 slices[1] can be >= slices[2]
    //   and slicing[0] can be 0 (no slicing, 6D2)
    if (slices[0] < 0 || slices[1] < 2 || slices[2] < 2)
    {
        throw FormatException(CR2Reader::kModuleName,
            m_fileName, "Error in slicing information array.");
    }
    return;
}

/// <summary>
/// Correct parameters base on specific cam models
/// </summary>
/// <param name="height">Height of the sensor</param>
/// <param name="slices">Slicing information array</param>
void CR2Reader::modelCorrect(
    int width, int &height, int(&slices)[3])
{
    string model;
    const bool modelTag = m_dirs[0].getTag(TiffTag::ID::Model, model);
    if (modelTag == true)
    {
        // Intentionally using plain names instead of camera
        // constants, which would cause dependency on them...

        if (model.compare("Canon EOS 5D Mark IV") == 0 ||
            model.compare("Canon EOS 5DS R") == 0 ||
            model.compare("Canon EOS 5DS") == 0)
        {
            height <<= 1;
            slices[2] = width >> 1;
        }
        else if (model.compare("Canon EOS 6D Mark II") == 0)
        {
            height <<= 1;
        }
    }
    return;
}

/// <summary>
/// Read and decompress RAW image data values
/// </summary>
/// <param name="img">Image for storing the resulting data</param>
/// <param name="slices">Information about data slicing</param>
/// <param name="dht">Huffman tables header</param>
/// <param name="sof3">Start of frame header</param>
/// <param name="sos">Start of scan header</param>
void CR2Reader::readRawImage(Array2D<uint16_t>& img, const int (&slices)[3],
    const DHTHeader& dht, const SOF3Header& sof3, const SOSHeader& sos)
{
    Array2D<uint16_t> dimg(sof3.samples * sof3.components, sof3.lines + 1);
    const uint16_t pval = 1 << (sof3.samplePrec - 1);

    if (sof3.components == 4) // Most common case
    {
        dimg[0][0] = pval;
        dimg[0][1] = pval;
        dimg[0][2] = pval;
        dimg[0][3] = pval;
        decode4(dimg, dht, sof3, sos); // Create image
        unslice(img, dimg, slices);
    }
    else // Generic case
    {
        for (int c = 0; c < sof3.components; c++)
            dimg[0][c] = pval;
        decode(dimg, dht, sof3, sos); // Create image
        unslice(img, dimg, slices);
    }
    return;
}

/// <summary>
/// Decode image data stored in RAW
/// </summary>
/// <param name="dimg">Decoded image before unslicing</param>
/// <param name="dht">Huffman tables header</param>
/// <param name="sof3">Start of frame header</param>
/// <param name="sos">Start of scan header</param>
inline void CR2Reader::decode(Array2D<uint16_t> &dimg,
    const DHTHeader &dht, const SOF3Header &sof3, const SOSHeader &sos)
{
    int prev[4];

    for (int line = 1; line <= sof3.lines; line++)
    {
        // Restar prev value for next line decoding
        for (int c = 0; c < sof3.components; c++)
        {
            prev[c] = dimg[line - 1][c];
        }
        // Decode next line
        for (int col = 0; col < sof3.samples; col++)
        {
            for (int c = 0; c < sof3.components; c++)
            {
                const HuffTree &ht = dht.huff[sos.tableSel[c][0]];
                prev[c] += decodeDiffValue(ht);
                dimg[line][sof3.components * col + c] = static_cast<uint16_t>(prev[c]);
            }
        }
    }
    return;
}

/// <summary>
/// Decode image data stored in RAW special for most common config
/// with 4 value components.
/// </summary>
/// <param name="dimg">Decoded image before unslicing</param>
/// <param name="dht">Huffman tables header</param>
/// <param name="sof3">Start of frame header</param>
/// <param name="sos">Start of scan header</param>
inline void CR2Reader::decode4(Array2D<uint16_t>& dimg,
    const DHTHeader& dht, const SOF3Header& sof3, const SOSHeader& sos)
{
    const int samples4 = 4 * sof3.samples;
    uint16_t* p = dimg[0]; // Previous line

    for (int line = 1; line <= sof3.lines; line++)
    {
        // Restart prev value for next line decoding
        int prev[4] = {p[0], p[1], p[2], p[3]};

        // Decode next line
        for (int col = 0; col < samples4; col += 4)
        {
            // Make prev to next cycle
            p = dimg[line];

            // Build up prev vector
            prev[0] += decodeDiffValue(dht.huff[sos.tableSel[0][0]]);
            prev[1] += decodeDiffValue(dht.huff[sos.tableSel[1][0]]);
            prev[2] += decodeDiffValue(dht.huff[sos.tableSel[2][0]]);
            prev[3] += decodeDiffValue(dht.huff[sos.tableSel[3][0]]);

            // Store image value
            p[col] = static_cast<uint16_t>(prev[0]);
            p[col + 1] = static_cast<uint16_t>(prev[1]);
            p[col + 2] = static_cast<uint16_t>(prev[2]);
            p[col + 3] = static_cast<uint16_t>(prev[3]);
        }
    }
    return;
}

/// <summary>
/// Unslicing decoded image data
/// </summary>
/// <param name="img">Final output image to be filled</param>
/// <param name="dimg">Decode image values</param>
/// <param name="slices">Information about slicing</param>
inline void CR2Reader::unslice(Array2D<uint16_t> &img,
    const Array2D<uint16_t> &ximg, const int(&slices)[3])
{
    const int height = img.getHeight(), sliceCount = slices[0];
    const uint16_t *p = ximg[1];

    for (int slice = 0, colBase = 0; slice <= sliceCount; slice++)
    {
        const int sliceWith = (slice >= sliceCount) ? slices[2] : slices[1];
        const int endCol = colBase + sliceWith;

        for (int row = 0; row < height; row++)
        {
            for (int col = colBase; col < endCol; col++)
            {
                img[row][col] = *p++;
            }
        }
        colBase += sliceWith;
    }
    return;
}

/// <summary>
/// Decode one difference value
/// </summary>
/// <param name="htree">Component associated huffman tree</param>
/// <returns>Decoded difference value</returns>
int CR2Reader::decodeDiffValue(const HuffTree& htree)
{
    HuffTree::iterator hi = htree.begin();
    int len = 0, diff = 0;

    // Traverse huffman tree
    while (hi != HuffTree::end && hi.leaf(len) == false)
    {
        const int x = getbit();
        hi.next(x);
    }
    if (hi != HuffTree::end)
    {
        if (len > 0) // Calculate difference codes
        {
            diff = getbits(len);
            if ((1 << (len - 1)) > diff)
                diff -= (1 << len) - 1;
        }
        return diff;
    }
    throw FormatException(CR2Reader::kModuleName, m_fileName,
        "Bad huffman code prefix in the data section.");
}

/// <summary>
/// Masking of bit at position (index)
/// </summary>
const uint8_t CR2Reader::kBitMasks[] = {
    0, 1, 2, 4, 8, 16, 32, 64, 128
};

/// <summary>
/// Get single bit from the stream
/// </summary>
/// <returns>Zero for 0. Otherwise nonzero for 1.</returns>
int CR2Reader::getbit()
{
    if (m_availBits <= 0)
        loadNextByte(); // Load new 8bits
    const int val = m_bitbuff & CR2Reader::kBitMasks[m_availBits];
    m_availBits--; // Move selector to next bit
    return val;
}

/// <summary>
/// Get requested number of bits from the stream
/// </summary>
/// <param name="bits">Number of bits to be extracted</param>
/// <returns>Zero for 0. Otherwise nonzero for 1.</returns>
/// <remarks>
/// Fastest variant when collecting data bitwise.
/// Other methods (dcraw, raw therapee, ..) working with
/// more bytes are slower than this. Every change make
/// the code slower. Maybe own buffering and reader could
/// work, but I havent tested it yet. Maybe in the future!
/// </remarks>
int CR2Reader::getbits(int bits)
{
    if (bits >= 0 && bits <= 24)
    {
        int val = 0;

        while (bits > 0)
        {
            if (m_availBits <= 0)
                loadNextByte();
            for (; m_availBits > 0 && bits > 0; bits--)
            {
                m_availBits--; // Pop first bit
                val = (val << 1) | ((m_bitbuff >> m_availBits) & 1);
            }
        }
        return val;
    }
    else throw FormatException(CR2Reader::kModuleName,
        m_fileName, "Too many or few bits for extraction.");
}

/// <summary>
/// Load next byte, 8bits, from the input stream
/// </summary>
void CR2Reader::loadNextByte()
{
    m_bitbuff = m_file.get();
    if (m_file.good() == true)
    {
        if (m_bitbuff == 0xff)
        {
            const int n = m_file.get();
            if (m_file.good() == false)
            {
                throw IOException(CR2Reader::kModuleName, m_fileName,
                    "Failed to extract an number of image data bits.");
            }
            if (n != 0)
            {
                throw FormatException(CR2Reader::kModuleName, m_fileName,
                    "After 0xff must follow 0x00 in the raw image data.");
            }
        }
        m_availBits = 8; // We read and have 8bits
        return;
    }
    throw IOException(CR2Reader::kModuleName, m_fileName,
        "Failed to extract an number of image data bits.");
}
