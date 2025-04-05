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

#include "Demosaic/Freeman.hpp"
#include "Demosaic/Bilinear.hpp"

#include "Structures/Image.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "Structures/Rect.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

#include <iostream>
using namespace std;

/// <summary>
/// Virtual empty destructor
/// </summary>
Demosaic::Freeman::~Freeman()
{ }

/// <summary>
/// Run freeman median demosaic algorithm
/// </summary>
/// <param name="img">Image to be demosaiced</param>
void Demosaic::Freeman::demosaic(Image &img)
{
    Bilinear bilinear; // Bilinear base
    bilinear.demosaic(img);

    // Dimensions
    m_ActiveArea = img.getCamProfile()->getActiveArea();
    const int width = m_ActiveArea.right - m_ActiveArea.left;
    const int height = m_ActiveArea.bottom - m_ActiveArea.top;

    // Channel differences arrays
    Array2D<double> diffRG(width, height), diffBG(width, height);
    calcChannelDiff(img, diffRG, diffBG);

    // Median filter on the data
    for (int i = 0; i < m_MedianIter; ++i)
    {
        median(diffRG);
        median(diffBG);
    }
    calcImageFromDiff(img, diffRG, diffBG);
    return;
}

/// <summary>
/// Print freeman demosaicing algorithm logo message
/// </summary>
/// <param name="os">Output stream to print</param>
void Demosaic::Freeman::printLogo(Logger &os) const
{
    os << "Freeman median demosaicing algorithm";
    os.newline();
    os << "with " << m_MedianIter << "x R-G and B-G median iterations";
    return;
}

/// <summary>
/// Calculate channel differences R-G, B-G
/// </summary>
/// <param name="img">Source image</param>
/// <param name="diffRG">Output R-G difference</param>
/// <param name="diffBG">Output B-G difference</param>
void Demosaic::Freeman::calcChannelDiff(
    const Image &img, Array2D<double> &diffRG, Array2D<double> &diffBG)
{
    #pragma omp parallel for
    for (int row = m_ActiveArea.top; row < m_ActiveArea.bottom; row++)
    {
        const int tr = row - m_ActiveArea.top;
        int col = m_ActiveArea.left, tc = 0;

        for (; col < m_ActiveArea.right; col++, tc++)
        {
            const Color::RGB64 value = img.getValue(row, col);
            diffRG[tr][tc] = value.r - value.g;
            diffBG[tr][tc] = value.b - value.g;
        }
    }
    return;
}

/// <summary>
/// Image recostruction from channel differences R-G, B-G
/// </summary>
/// <param name="img">Target image for reconstruction</param>
/// <param name="diffRG">Input R-G difference</param>
/// <param name="diffBG">Input B-G difference</param>
void Demosaic::Freeman::calcImageFromDiff(
    Image &img, const Array2D<double> &diffRG, const Array2D<double> &diffBG)
{
    #pragma omp parallel for
    for (int row = m_ActiveArea.top; row < m_ActiveArea.bottom; row++)
    {
        const int tr = row - m_ActiveArea.top;
        int col = m_ActiveArea.left, tc = 0;

        for (; col < m_ActiveArea.right; col++, tc++)
        {
            Color::RGB64 result, value;
            value = img.getValue(row, col);

            // Like DCRAW, and not complete right, but works...
            result.r = diffRG[tr][tc] + value.g;
            result.g = value.g;
            result.b = diffBG[tr][tc] + value.g;
            img.setValue(row, col, result);
        }
    }
    return;
}

/// <summary>
/// Compute median filter on the channel
/// </summary>
/// <param name="channel">Channel for medianing</param>
void Demosaic::Freeman::median(Array2D<double> &channel)
{
    const Array2D<double> data(channel);
    const int width = data.getWidth() - 1, height = data.getHeight() - 1;

    #pragma omp parallel for
    for (int row = 1; row < height; row++)
    {
        for (int col = 1; col < width; col++)
        {
            double v[9];
            v[0] = data[row][col];
            v[1] = data[row][col - 1];
            v[2] = data[row][col + 1];
            v[3] = data[row - 1][col];
            v[4] = data[row - 1][col - 1];
            v[5] = data[row - 1][col + 1];
            v[6] = data[row + 1][col];
            v[7] = data[row + 1][col - 1];
            v[8] = data[row + 1][col + 1];
            qsort(v, 9, sizeof(double), Utils::compareDouble);
            channel[row][col] = v[4];
        }
    }
    return;
}
