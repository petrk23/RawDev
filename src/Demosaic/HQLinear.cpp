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

#include "HQLinear.hpp"

/*
HIGH-QUALITY LINEAR INTERPOLATION
FOR DEMOSAICING OF BAYER-PATTERNED COLOR IMAGES

Best linear demosaicing algorithm. There is at
the limit what could be done with linear filter.
*/

#include "Structures/Image.hpp"
#include "Structures/Rect.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "CamProfiles/CFAPattern.hpp"
#include "Logger.hpp"

#include <iostream>
using namespace std;

/// <summary>
/// Virtual empty destructor
/// </summary>
Demosaic::HQLinear::~HQLinear()
{ }

/// <summary>
/// High-quality linear demosaicing algorithm
/// </summary>
/// <param name="img">Image to be demosaiced</param>
void Demosaic::HQLinear::demosaic(Image &img)
{
    const Image srcImg(img); // Save source image
    constexpr int padding = 2;
    const Rect active = img.getCamProfile()->getActiveArea();
    const int brow = active.top + padding, erow = active.bottom - padding,
        bcol = active.left + padding, ecol = active.right - padding;
    const CFAPattern cfa = img.getCamProfile()->getCFAPattern();
    Color::RGB64 value; // Current pixel value

    #pragma omp parallel for private(value)
    for (int row = brow; row < erow; row++)
    {
        for (int col = bcol; col < ecol; col++)
        {
            switch (cfa(row, col))
            {
            case CFAPattern::Color::RED:
                value.r = srcImg.getValueR(row, col);
                value.g = interGreenFromRed(srcImg, row, col);
                value.b = interBlueFromRed(srcImg, row, col);
                break;
            case CFAPattern::Color::BLUE:
                value.r = interRedFromBlue(srcImg, row, col);
                value.g = interGreenFromBlue(srcImg, row, col);
                value.b = srcImg.getValueB(row, col);
                break;
            case CFAPattern::Color::GREEN_R:
                value.r = interRedFromGreenR(srcImg, row, col);
                value.g = srcImg.getValueG(row, col);
                value.b = interBlueFromGreenR(srcImg, row, col);
                break;
            default: // CFAPattern::Color::GREEN_B
                value.r = interRedFromGreenB(srcImg, row, col);
                value.g = srcImg.getValueG(row, col);
                value.b = interBlueFromGreenB(srcImg, row, col);
                break;
            }
            img.setValue(row, col, value); // Save the result
        } // col loop
    } // row loop
    return;
}

/// <summary>
/// Print demosaic logo message
/// </summary>
/// <param name="os">Output stream for print</param>
void Demosaic::HQLinear::printLogo(Logger &os) const
{
    os << "High-Quality linear demosaicing algorithm";
    return;
}

/// <summary>
/// Interpolate Green from Red(Pattern No. 1)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interGreenFromRed(const Image &img, int row, int col)
{
    double gsum, rsum;

    // Green for linear interpolation
    gsum = img.getValueG(row - 1, col);
    gsum += img.getValueG(row, col + 1);
    gsum += img.getValueG(row + 1, col);
    gsum += img.getValueG(row, col - 1);

    // Red for gradient correction
    rsum = 4 * img.getValueR(row, col);
    rsum -= img.getValueR(row - 2, col);
    rsum -= img.getValueR(row, col + 2);
    rsum -= img.getValueR(row + 2, col);
    rsum -= img.getValueR(row, col - 2);

    // Resulting interpolated green
    return (2 * gsum + rsum) / 8;
}

/// <summary>
/// Interpolate Green from Blue (Pattern No. 2)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interGreenFromBlue(const Image &img, int row, int col)
{
    double gsum, bsum;

    // Green for linear interpolation
    gsum = img.getValueG(row - 1, col);
    gsum += img.getValueG(row, col + 1);
    gsum += img.getValueG(row + 1, col);
    gsum += img.getValueG(row, col - 1);

    // Red for gradient correction
    bsum = 4 * img.getValueR(row, col);
    bsum -= img.getValueR(row - 2, col);
    bsum -= img.getValueR(row, col + 2);
    bsum -= img.getValueR(row + 2, col);
    bsum -= img.getValueR(row, col - 2);

    // Resulting interpolated green
    return (2 * gsum + bsum) / 8;
}

/// <summary>
/// Interpolate Red from Green on Red line (Pattern No. 3)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interRedFromGreenR(const Image &img, int row, int col)
{
    double rsum, gsum;

    // Blinear interpolation of red
    rsum = img.getValueR(row, col - 1) + img.getValueR(row, col + 1);
    rsum *= 4;

    // Gradient correction of Green
    gsum = 5 * img.getValueG(row, col);
    gsum += (img.getValueG(row - 2, col) + img.getValueG(row + 2, col)) / 2;
    gsum -= img.getValueG(row + 1, col - 1) + img.getValueG(row + 1, col + 1)
        + img.getValueG(row - 1, col - 1) + img.getValueG(row - 1, col + 1)
        + img.getValueG(row, col - 2) + img.getValueG(row, col + 2);
    return (rsum + gsum) / 8;
}

/// <summary>
/// Interpolate Red from Green on Blue line (Pattern No. 4)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interRedFromGreenB(const Image &img, int row, int col)
{
    double rsum = img.getValueR(row - 1, col) + img.getValueR(row + 1, col);
    rsum *= 4;

    double gsum = 5 * img.getValueG(row, col);
    gsum += (img.getValueG(row, col - 2) + img.getValueG(row, col + 2)) / 2;
    gsum -= img.getValueG(row + 1, col - 1) + img.getValueG(row + 1, col + 1)
        + img.getValueG(row - 1, col - 1) + img.getValueG(row - 1, col + 1)
        + img.getValueG(row - 2, col) + img.getValueG(row + 2, col);
    return (rsum + gsum) / 8;
}

/// <summary>
/// Interpolate Red from Blue (Pattern No. 5)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interRedFromBlue(const Image &img, int row, int col)
{
    double rsum, bsum;

    // Red for linear interpolation
    rsum = img.getValueR(row + 1, col - 1);
    rsum += img.getValueR(row + 1, col + 1);
    rsum += img.getValueR(row - 1, col + 1);
    rsum += img.getValueR(row - 1, col - 1);

    // Blue for gradient correct
    bsum = 4 * img.getValueB(row, col);
    bsum -= img.getValueB(row - 2, col);
    bsum -= img.getValueB(row, col + 2);
    bsum -= img.getValueB(row - 2, col);
    bsum -= img.getValueB(row, col - 2);

    //  Resulting interpolated red
    return (2 * rsum + (3.0 / 2) * bsum) / 8;
}

/// <summary>
/// Interpolate Blue from Green on Blue line (Pattern No. 6)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interBlueFromGreenB(const Image &img, int row, int col)
{
    double bsum, gsum;

    // Blinear interpolation of red
    bsum = img.getValueB(row, col - 1) + img.getValueB(row, col + 1);
    bsum *= 4;

    // Gradient correction of Green
    gsum = 5 * img.getValueG(row, col);
    gsum += (img.getValueG(row - 2, col) + img.getValueG(row + 2, col)) / 2;
    gsum -= img.getValueG(row + 1, col - 1) + img.getValueG(row + 1, col + 1)
        + img.getValueG(row - 1, col - 1) + img.getValueG(row - 1, col + 1)
        + img.getValueG(row, col - 2) + img.getValueG(row, col + 2);
    return (bsum + gsum) / 8;
}

/// <summary>
/// Interpolate Blue from Green on Red line (Pattern No. 7)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interBlueFromGreenR(const Image &img, int row, int col)
{
    double bsum = img.getValueB(row - 1, col) + img.getValueB(row + 1, col);
    bsum *= 4;

    double gsum = 5 * img.getValueG(row, col);
    gsum += (img.getValueG(row, col - 2) + img.getValueG(row, col + 2)) / 2;
    gsum -= img.getValueG(row + 1, col - 1) + img.getValueG(row + 1, col + 1)
        + img.getValueG(row - 1, col - 1) + img.getValueG(row - 1, col + 1)
        + img.getValueG(row - 2, col) + img.getValueG(row + 2, col);
    return (bsum + gsum) / 8;
}

/// <summary>
/// Interpolate Blue from Red(Pattern No. 8)
/// </summary>
/// <param name="img">Source image</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
/// <returns>Interpolated value</returns>
double Demosaic::HQLinear::interBlueFromRed(const Image &img, int row, int col)
{
    double bsum, rsum;

    // Blue for linear interpolation
    bsum  = img.getValueB(row + 1, col - 1);
    bsum += img.getValueB(row + 1, col + 1);
    bsum += img.getValueB(row - 1, col + 1);
    bsum += img.getValueB(row - 1, col - 1);

    // Red for gradient correct
    rsum  = 4 * img.getValueR(row, col);
    rsum -= img.getValueR(row - 2, col);
    rsum -= img.getValueR(row, col + 2);
    rsum -= img.getValueR(row - 2, col);
    rsum -= img.getValueR(row, col - 2);

    //  Resulting interpolated blue
    return (2 * bsum + (3.0 / 2) * rsum) / 8;
}
