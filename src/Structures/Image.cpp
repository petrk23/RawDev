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

#include "CamProfiles/CamProfile.hpp"
#include "Exception.hpp"
#include "Image.hpp"
#include "ImageIO/CR2Reader.hpp"
#include "Path.hpp"
#include "Rect.hpp"

#include <sstream>
#include <omp.h>

/// <summary>
/// Load image from Canon CR2 raw file
/// </summary>
/// <param name="inputFile">Path to the input file</param>
/// <exception cref="IOException, FormatException">
/// In case of errors IOException or Format exception is thrown.
/// </exception>
void Image::loadCR2(const Path& inputFile, double temp)
{
    CR2Reader file(inputFile);
    file.open(); // Open and store metadata
    setupMetadata(file, temp);

    Array2D<uint16_t> raw;
    file.read(raw);    // Load the main image data
    storeRawData(raw); // Store raw data into image
    file.close();
}

/// <summary>
/// Setup image metadata from raw file
/// </summary>
/// <param name="file">Source raw file reader</param>
void Image::setupMetadata(const CR2Reader& file, double temp)
{
    // Load model
    std::string model;
    if (file.getModel(model) == false) {
        throw UnsupportedCamException("RawInfo",
            "Unnamed cameras are not supported.");
    }

    // Select camera profile by name
    m_CamProfile = CamProfile::MakeCamProfile(model, temp);
    if (m_CamProfile == nullptr) {
        std::stringstream ss;
        ss << "The camera '" << model << "' ist not supported yet.";
        throw UnsupportedCamException("RawInfo", ss.str());
    }
}

/// <summary>
/// Store read raw data into image
/// </summary>
/// <param name="img">Source raw image data</param>
void Image::storeRawData(Array2D<uint16_t>& img)
{
    const CFAPattern cfa = m_CamProfile->getCFAPattern();
    const int width = img.getWidth(), height = img.getHeight();

    // Allocate image channel data
    m_red = Array2D<double>(width, height, 0);
    m_green = Array2D<double>(width, height, 0);
    m_blue = Array2D<double>(width, height, 0);

    #pragma omp parallel for
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            const double value = static_cast<double>(img[row][col]);

            switch (cfa(row, col)) {
            case CFAPattern::Color::RED:
                m_red[row][col] = value;
                break;
            case CFAPattern::Color::BLUE:
                m_blue[row][col] = value;
                break;
            default: // GREEN_R || GREEN_B
                m_green[row][col] = value;
                break;
            }
            // Do not call setValue method as this would
            // normalize to range 0.0-1.0 and discart RAW
            // values resulting in black image.
        }
    }
}

/// <summary>
/// Convert image to 16bit RGB image
/// </summary>
/// <param name="img16">Output image</param>
/// <remarks>
/// Is needed to call before saving to TIFF
/// </remarks>
void Image::convert16(Array2D<Color::RGB16>& img16, bool noCrop) const
{
    Rect crop; // Current used crop
    if (noCrop == true)
        crop = Rect::Create(Point(0, 0), getWidth(), getHeight());
    else
        crop = m_CamProfile->getCrop();

    // Alloc new croped image
    img16 = Array2D<Color::RGB16>(crop.getWidth(), crop.getHeight());

    #pragma omp parallel for
    for (int row = crop.top; row < crop.bottom; row++) {
        for (int col = crop.left; col < crop.right; col++) {
            int xrow = row - crop.top, xcol = col - crop.left;

            img16[xrow][xcol].r = Image::doubleTo16(m_red[row][col]);
            img16[xrow][xcol].g = Image::doubleTo16(m_green[row][col]);
            img16[xrow][xcol].b = Image::doubleTo16(m_blue[row][col]);
        }
    }
}

void Image::convert8(Array2D<Color::RGB8>& img8, bool noCrop) const
{
    Rect crop; // Current used crop
    if (noCrop == true)
        crop = Rect::Create(Point(0, 0), getWidth(), getHeight());
    else
        crop = m_CamProfile->getCrop();

    // Alloc new croped image
    img8 = Array2D<Color::RGB8>(crop.getWidth(), crop.getHeight());

    #pragma omp parallel for
    for (int row = crop.top; row < crop.bottom; row++) {
        for (int col = crop.left; col < crop.right; col++) {
            int xrow = row - crop.top, xcol = col - crop.left;

            img8[xrow][xcol].r = Image::doubleTo8(m_red[row][col]);
            img8[xrow][xcol].g = Image::doubleTo8(m_green[row][col]);
            img8[xrow][xcol].b = Image::doubleTo8(m_blue[row][col]);
        }
    }
}
