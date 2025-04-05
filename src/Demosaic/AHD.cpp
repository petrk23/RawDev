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

#include "AHD.hpp"

#include "Structures/Array2D.hpp"
#include "Structures/Mat3x3.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "CamProfiles/CFAPattern.hpp"
#include "Color.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

#include <omp.h>

/// <summary>
/// Virtual empty destructor
/// </summary>
Demosaic::AHD::~AHD()
{ }

/// <summary>
/// Adaptive homogeneity demosaicing algorithm
/// </summary>
/// <param name="img">Image to be demosaiced</param>
void Demosaic::AHD::demosaic(Image& img)
{
    // Matrix for conversion to Lab
    const Mat3x3 cam2XYZ = img.getCamProfile()->getColorMatrix().inverse();
    const Rect active = img.getCamProfile()->getActiveArea();

    const Image raw(img); // Fixed raw image data
    const int xmargin = active.left + 2, ymargin = active.top + 2;
    const int xTileCount = calcTileCount(active.right - active.left - 7, xTileSize - 6);
    const int tileCount = xTileCount
        * calcTileCount(active.bottom - active.top - 7, yTileSize - 6);
    const CFAPattern cfa = img.getCamProfile()->getCFAPattern();

    #pragma omp parallel
    {
        // Needed data structures (tile 512 cca. 25MB per core)
        Array2D<Color::RGB64> himg(xTileSize, yTileSize);
        Array2D<Color::RGB64> vimg(xTileSize, yTileSize);
        Array2D<Color::CIELab> himgLab(xTileSize, yTileSize);
        Array2D<Color::CIELab> vimgLab(xTileSize, yTileSize);
        Array2D<homo_t> hhomo(xTileSize, yTileSize);
        Array2D<homo_t> vhomo(xTileSize, yTileSize);

        #pragma omp for schedule(dynamic) nowait
        for (int t = 0; t < tileCount; t++)
        {
            const div_t tmpdiv = std::div(t, xTileCount);
            const int rbase = ymargin + (yTileSize - 6) * tmpdiv.quot;
            const int cbase = xmargin + (yTileSize - 6) * tmpdiv.rem;

            // AHD demosaicing algorithm
            interGreen(raw, cfa, rbase, cbase, himg, vimg);
            interRedBlue(raw, cfa, rbase, cbase, himg, himgLab, cam2XYZ);
            interRedBlue(raw, cfa, rbase, cbase, vimg, vimgLab, cam2XYZ);
            generateHomogenityMasks(raw, rbase, cbase,
                himgLab, hhomo, vimgLab, vhomo);
            composeOutput(img, rbase, cbase, himg, hhomo, vimg, vhomo);
        }
    }
    return;
}

/// <summary>
/// Print demosaic logo message
/// </summary>
/// <param name="os">Output stream for print</param>
void Demosaic::AHD::printLogo(Logger &os) const
{
    os << "Adaptive homogeneity demosaicing algorithm";
    return;
}

/// <summary>
/// Horizontal and vertical green tile interpolation
/// </summary>
/// <param name="img">Source image</param>
/// <param name="cfa">CFA pattern</param>
/// <param name="brow">Tile base row</param>
/// <param name="bcol">Tile base column</param>
/// <param name="himg">Horizontal interpolated image</param>
/// <param name="vimg">Vertical interpolated image</param>
void Demosaic::AHD::interGreen(
    const Image& img, const CFAPattern &cfa, int brow, int bcol,
    Array2D<Color::RGB64>& himg, Array2D<Color::RGB64>& vimg)
{
    const int erow = std::min(brow + yTileSize, img.getHeight() - 2);
    const int ecol = std::min(bcol + xTileSize, img.getWidth() - 2);
    Image::Channel ctype; int cfaShift; // Row information
    cfaShift = initRowInfo(cfa, brow, bcol, ctype);

    // Interpolate green values
    for (int row = brow; row < erow; row++)
    {
        ctype = (ctype == Image::Channel::RED)
            ? Image::Channel::BLUE : Image::Channel::RED;
        cfaShift ^= 1; // Invert the CFA shift

        for (int col = bcol + cfaShift; col < ecol; col += 2)
        {
            double val, p, q; // Computation variables

            // Base element load (center)
            const double mid = img.getValueX(row, col, ctype);

            // Horizontal interpolation
            p = img.getValueG(row, col - 1);
            q = img.getValueG(row, col + 1);
            val = 0.25 * (2 * (mid + p + q) - img.getValueX(row, col - 2, ctype)
                - img.getValueX(row, col + 2, ctype));
            himg[row - brow][col - bcol].g = Utils::median(val, p, q);

            // Vertical interpolation
            p = img.getValueG(row - 1, col);
            q = img.getValueG(row + 1, col);
            val = 0.25 * (2 * (mid + p + q) - img.getValueX(row - 2, col, ctype)
                - img.getValueX(row + 2, col, ctype));
            vimg[row - brow][col - bcol].g = Utils::median(val, p, q);
        }
    }
    return;
}

/// <summary>
/// Initialize row info for green interpolation
/// </summary>
/// <param name="cfa">CFA pattern</param>
/// <param name="brow">Tile base row</param>
/// <param name="bcol">Tile base column</param>
/// <param name="ctype">Resulting image channel type</param>
/// <returns>Shift to next green pixel on row</returns>
/// <remarks>
/// Note: ctype and cfaShift must be returned inverted to work!
/// </remarks>
inline int Demosaic::AHD::initRowInfo(
    const CFAPattern &cfa, int brow, int bcol, Image::Channel &ctype)
{
    int cfaShift; // Inverted CFA shift to next nongreen filter

    switch (cfa(brow, bcol))
    {
    case CFAPattern::Color::RED:
        ctype = Image::Channel::BLUE;
        cfaShift = 1;
        break;
    case CFAPattern::Color::BLUE:
        ctype = Image::Channel::RED;
        cfaShift = 1;
        break;
    case CFAPattern::Color::GREEN_R:
        ctype = Image::Channel::BLUE;
        cfaShift = 0;
        break;
    default: // CFAPattern::Color::GREEN_B
        ctype = Image::Channel::RED;
        cfaShift = 0;
        break;
    }
    return cfaShift;
}

/// <summary>
/// Bilinear interpolate Red and Blue tile channels
/// </summary>
/// <param name="img">Source image</param>
/// <param name="cfa">CFA pattern</param>
/// <param name="brow">Tile base row</param>
/// <param name="bcol">Tile base column</param>
/// <param name="timg">Interpolated target image</param>
/// <param name="imgLab">Output LAB image</param>
/// <param name="cam2XYZ">Prepared matrix for LAB conversion</param>
void Demosaic::AHD::interRedBlue(
    const Image& img, const CFAPattern &cfa, int brow, int bcol,
    Array2D<Color::RGB64>& timg, Array2D<Color::CIELab>& imgLab,
    const Mat3x3& cam2XYZ)
{
    Color::RGB64 val; // Pixel value variable

    constexpr int padding = 1; // Border inside tile
    const int erow = std::min(brow + yTileSize - padding, img.getHeight() - 3);
    const int ecol = std::min(bcol + xTileSize - padding, img.getWidth() - 3);
    brow++; bcol++; // Advance by padding

    // Interpolate RB values
    for (int row = brow, tr = padding; row < erow; row++, tr++)
    {
        for (int col = bcol, tc = padding; col < ecol; col++, tc++)
        {
            switch (cfa(row, col))
            {
            case CFAPattern::Color::RED:
                val = AHD::interOnRed(img, row, col, timg, tr, tc);
                break;
            case CFAPattern::Color::BLUE:
                val = AHD::interOnBlue(img, row, col, timg, tr, tc);
                break;
            case CFAPattern::Color::GREEN_R:
                val = AHD::interOnGreenR(img, row, col, timg, tr, tc);
                break;
            default: // CFAPattern::Color::GREEN_B
                val = AHD::interOnGreenB(img, row, col, timg, tr, tc);
                break;
            }
            timg[tr][tc] = val; // Save interpolated
            imgLab[tr][tc] = camRGB2Lab(cam2XYZ, val);
        }
    }
    return;
}

/// <summary>
/// Generate homogeneity from Lab images
/// </summary>
/// <param name="img">Source image</param>
/// <param name="brow">Tile base row</param>
/// <param name="bcol">Tile base column</param>
/// <param name="himgLab">Horizontaly interpolated LAB image</param>
/// <param name="hhomo">Horizontal homogeneity</param>
/// <param name="vimgLab">Vertical interpolated LAB image</param>
/// <param name="vhomo">Vertical homogeneity</param>
void Demosaic::AHD::generateHomogenityMasks(
    const Image& img, const int brow, const int bcol,
    const Array2D<Color::CIELab>& himgLab, Array2D<homo_t>& hhomo,
    const Array2D<Color::CIELab>& vimgLab, Array2D<homo_t>& vhomo)
{
    constexpr int padding = 2; // Border inside tile
    const int erow = std::min(brow + yTileSize - padding, img.getHeight() - 4) - brow;
    const int ecol = std::min(bcol + xTileSize - padding, img.getWidth() - 4) - bcol;

    for (int tr = padding; tr < erow; tr++) {
        for (int tc = padding; tc < ecol; tc++) {

            // Precalculate Lab differences
            double ldiff[2][4], cdiff[2][4];
            const Color::CIELab hdata[] = {
                himgLab[tr][tc],
                himgLab[tr][tc - 1], himgLab[tr][tc + 1],
                himgLab[tr - 1][tc], himgLab[tr + 1][tc]
            };
            const Color::CIELab vdata[] = {
                vimgLab[tr][tc],
                vimgLab[tr][tc - 1], vimgLab[tr][tc + 1],
                vimgLab[tr - 1][tc], vimgLab[tr + 1][tc]
            };
            for (int d = 1; d <= 4; d++) {
                ldiff[0][d - 1] = hdata[0].dl(hdata[d]);
                ldiff[1][d - 1] = vdata[0].dl(vdata[d]);
                cdiff[0][d - 1] = hdata[0].dc(hdata[d]);
                cdiff[1][d - 1] = vdata[0].dc(vdata[d]);
            }

            // Adaptive estimate el
            const double el = std::min(
                std::max(ldiff[0][0], ldiff[0][1]),
                std::max(ldiff[1][2], ldiff[1][3])
            );
            // Adaptive estimate ec
            const double ec = std::min(
                std::max(cdiff[0][0], cdiff[0][1]),
                std::max(cdiff[1][2], cdiff[1][3])
            );

            // Evaluate differences of neighborhood pixels
            int hh = 0, vh = 0;
            for (int d = 0; d < 4; d++) {
                hh += (ldiff[0][d] <= el) & (cdiff[0][d] <= ec);
                vh += (ldiff[1][d] <= el) & (cdiff[1][d] <= ec);
            }
            hhomo[tr][tc] = static_cast<homo_t>(hh);
            vhomo[tr][tc] = static_cast<homo_t>(vh);
        }
    }
    return;
}

/// <summary>
/// Compose final demosaiced image from horizontal and vertical image
/// </summary>
/// <param name="img">Source image</param>
/// <param name="brow">Tile base row</param>
/// <param name="bcol">Tile base column</param>
/// <param name="himg">Horizontal interpolated image</param>
/// <param name="hhomo">Horizontal homogeneity</param>
/// <param name="vimg">Vertical interpolated image</param>
/// <param name="vhomo">Vertical homogeneity</param>
void Demosaic::AHD::composeOutput(
    Image& img, const int brow, const int bcol,
    const Array2D<Color::RGB64>& himg, const Array2D<homo_t>& hhomo,
    const Array2D<Color::RGB64>& vimg, const Array2D<homo_t>& vhomo)
{
    constexpr int padding = 3; // Border inside tile
    const int erow = std::min(brow + yTileSize - padding, img.getHeight() - 5) - brow;
    const int ecol = std::min(bcol + xTileSize - padding, img.getWidth() - 5) - bcol;
    Color::RGB64 value;

    for (int tr = padding; tr < erow; tr++)
    {
        for (int tc = padding; tc < ecol; tc++)
        {
            homo_t hhm, vhm; // Homogeneity for that pixel
            averageHomogenity(hhomo, vhomo, tr, tc, hhm, vhm);

            // Evaluate homogenity
            if (hhm > vhm)
                value = himg[tr][tc]; // Value is horizontal
            else if (hhm < vhm)
                value = vimg[tr][tc]; // Value is vertical
            else
            {
                const Color::RGB64 hval = himg[tr][tc], vval = vimg[tr][tc];
                value.r = 0.5 * (hval.r + vval.r); // Homogenity doesn't helped
                value.g = 0.5 * (hval.g + vval.g); // Average values
                value.b = 0.5 * (hval.b + vval.b);
            }
            img.setValue(brow + tr, bcol + tc, value);
        }
    }
    return;
}

/// <summary>
/// Box 3x3 convolution smoothing of homogenity for one pixel
/// </summary>
/// <param name="hhomo">Horizontal homogeneity</param>
/// <param name="vhomo">Vertical homogeneity</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <param name="hhm">Smoothe horizontal homogeneity value</param>
/// <param name="vhm">Smoothe vertical homogeneity value</param>
inline void Demosaic::AHD::averageHomogenity(
    const Array2D<homo_t>& hhomo, const Array2D<homo_t>& vhomo,
    int row, int col, homo_t& hhm, homo_t& vhm)
{
    const int lastrow = row + 1, firstcol = col - 1, lastcol = col + 1;
    int hval = 0, vval = 0;

    // Convolution with box kernel
    for (row -= 1; row <= lastrow; row++)
    {
        for (col = firstcol; col <= lastcol; col++)
        {
            hval += hhomo[row][col];
            vval += vhomo[row][col];
        }
    }
    hhm = static_cast<homo_t>(hval);
    vhm = static_cast<homo_t>(vval);
    return;
}

/// <summary>
/// Bilinear interpolation of Red and Blue on Green (Red line)
/// </summary>
/// <param name="img">Full source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <param name="tileimg">Image working tile</param>
/// <param name="tr">Row inside tiled image</param>
/// <param name="tc">Column inseide tiled image</param>
/// <returns>Interpolated pixel value</returns>
inline Color::RGB64 Demosaic::AHD::interOnGreenR(
    const Image& img, const int row, const int col,
    const Array2D<Color::RGB64>& tileimg, const int tr, const int tc)
{
    Color::RGB64 val;
    val.g = img.getValueG(row, col); // Green is known

    // R = G * (R1 - G1 + R2 - G2)/2
    val.r = val.g + 0.5 * (img.getValueR(row, col - 1) - tileimg[tr][tc - 1].g
        + img.getValueR(row, col + 1) - tileimg[tr][tc + 1].g);

    // B = G * (B1 - G1 + B2 - G2)/2
    val.b = val.g + 0.5 * (img.getValueB(row - 1, col) - tileimg[tr - 1][tc].g
        + img.getValueB(row + 1, col) - tileimg[tr + 1][tc].g);
    return val;
}

/// <summary>
/// Bilinear interpolation of Red and Blue on Green (Blue line)
/// </summary>
/// <param name="img">Full source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <param name="tileimg">Image working tile</param>
/// <param name="tr">Row inside tiled image</param>
/// <param name="tc">Column inseide tiled image</param>
/// <returns>Interpolated pixel value</returns>
inline Color::RGB64 Demosaic::AHD::interOnGreenB(
    const Image& img, const int row, const int col,
    const Array2D<Color::RGB64>& tileimg, const int tr, const int tc)
{
    Color::RGB64 val;
    val.g = img.getValueG(row, col); // Green is known

    // B = G * (B1 - G1 + B2 - G2)/2
    val.b = val.g + 0.5 * (img.getValueB(row, col - 1) - tileimg[tr][tc - 1].g
        + img.getValueB(row, col + 1) - tileimg[tr][tc + 1].g);

    // R = G * (R1 - G1 + R2 - G2)/2
    val.r = val.g + 0.5 * (img.getValueR(row - 1, col) - tileimg[tr - 1][tc].g
        + img.getValueR(row + 1, col) - tileimg[tr + 1][tc].g);
    return val;
}

/// <summary>
/// Bilinear interpolation of Blue on Red (Red line)
/// </summary>
/// <param name="img">Full source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <param name="tileimg">Image working tile</param>
/// <param name="tr">Row inside tiled image</param>
/// <param name="tc">Column inseide tiled image</param>
/// <returns>Interpolated pixel value</returns>
inline Color::RGB64 Demosaic::AHD::interOnRed(
    const Image& img, const int row, const int col,
    const Array2D<Color::RGB64>& tileimg, const int tr, const int tc)
{
    Color::RGB64 val;
    val.r = img.getValueR(row, col);
    val.g = tileimg[tr][tc].g; // Interpolated G from prev. step

    // Interpolate B from diagonal Blue elements
    val.b = val.g + (img.getValueB(row - 1, col - 1) - tileimg[tr - 1][tc - 1].g
        + img.getValueB(row + 1, col + 1) - tileimg[tr + 1][tc + 1].g
        + img.getValueB(row + 1, col - 1) - tileimg[tr + 1][tc - 1].g
        + img.getValueB(row - 1, col + 1) - tileimg[tr - 1][tc + 1].g) * 0.25;
    return val;
}

/// <summary>
/// Bilinear interpolation of Red on Blue (Blue line)
/// </summary>
/// <param name="img">Full source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <param name="tileimg">Image working tile</param>
/// <param name="tr">Row inside tiled image</param>
/// <param name="tc">Column inseide tiled image</param>
/// <returns>Interpolated pixel value</returns>
inline Color::RGB64 Demosaic::AHD::interOnBlue(
    const Image& img, const int row, const int col,
    const Array2D<Color::RGB64>& tileimg, const int tr, const int tc)
{
    Color::RGB64 val;
    val.b = img.getValueB(row, col);
    val.g = tileimg[tr][tc].g; // Interpolated G from prev. step

    // Interpolate R from diagonal Red elements
    val.r = val.g + (img.getValueR(row - 1, col - 1) - tileimg[tr - 1][tc - 1].g
        + img.getValueR(row + 1, col + 1) - tileimg[tr + 1][tc + 1].g
        + img.getValueR(row + 1, col - 1) - tileimg[tr + 1][tc - 1].g
        + img.getValueR(row - 1, col + 1) - tileimg[tr - 1][tc + 1].g) * 0.25;
    return val;
}

/// <summary>
/// Convert camera RGB value to CIE Lab value
/// </summary>
/// <param name="cam2XYZ">Conversion matrix from camera to XYZ</param>
/// <param name="src">Source data for conversion</param>
/// <returns>Converted CIE LAB value</returns>
inline Color::CIELab
Demosaic::AHD::camRGB2Lab(const Mat3x3& cam2XYZ, const Color::RGB64& src)
{
    const auto input{Color::rgbTo<Color::CIEXYZ>(cam2XYZ, src)};
    Color::CIELab output;
    Color::xyz2Lab(input, output, Color::k_d50);
    return output;
}

/// <summary>
/// Calculate tile count to fill dimension
/// </summary>
/// <param name="dim"></param>
/// <param name="rts"></param>
/// <returns></returns>
inline int Demosaic::AHD::calcTileCount(int dim, int rts)
{
    const double tmpdim = ceil(static_cast<double>(dim) / (rts));
    return static_cast<int>(tmpdim);
}
