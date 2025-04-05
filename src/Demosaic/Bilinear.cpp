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

#include "Bilinear.hpp"

#include "Structures/Image.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "CamProfiles/CFAPattern.hpp"
#include "Logger.hpp"

/// <summary>
/// Virtual empty destructor
/// </summary>
Demosaic::Bilinear::~Bilinear()
{ }

/// <summary>
/// Bilinear demosaicing algorithm
/// </summary>
/// <param name="img"></param>
void Demosaic::Bilinear::demosaic(Image &img)
{
    constexpr int padding = 1;
    const Rect active = img.getCamProfile()->getActiveArea();
    const int erow = active.bottom - padding, ecol = active.right - padding;
    const CFAPattern cfa = img.getCamProfile()->getCFAPattern();

    #pragma omp parallel for
    for (int row = active.top + padding; row < erow; row++)
    {
        for (int col = active.left + padding; col < ecol; col++)
        {
            switch (cfa(row, col))
            {
            case CFAPattern::Color::RED:
                interGB(img, row, col); // R->GB
                break;
            case CFAPattern::Color::BLUE:
                interRG(img, row, col); // B->RG
                break;
            default: // GREEN_R || GREEN_B
                interRB(img, cfa, row, col); // G->RB
                break;
            }
        } // col loop
    } // row loop
    return;
}

/// <summary>
/// Print demosaic logo message
/// </summary>
/// <param name="os">Output stream for print</param>
void Demosaic::Bilinear::printLogo(Logger &os) const
{
    os << "Bilinear demosaicing algorithm";
    return;
}

/// <summary>
/// Interpolate Red and Green on Blue pixel
/// </summary>
/// <param name="img">Image for interpolation</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
void Demosaic::Bilinear::interRG(Image &img, int row, int col)
{
    Color::RGB64 value = img.getValue(row, col);

    // Diagonal surrounding (R)
    const Color::RGB64 r1 = img.getValue(row + 1, col + 1);
    const Color::RGB64 r2 = img.getValue(row - 1, col - 1);
    const Color::RGB64 r3 = img.getValue(row + 1, col - 1);
    const Color::RGB64 r4 = img.getValue(row - 1, col + 1);

    // Axis surrounding (G)
    const Color::RGB64 g1 = img.getValue(row, col + 1);
    const Color::RGB64 g2 = img.getValue(row, col - 1);
    const Color::RGB64 g3 = img.getValue(row + 1, col);
    const Color::RGB64 g4 = img.getValue(row - 1, col);

    // Interpolate
    value.r = (r1.r + r2.r + r3.r + r4.r) / 4;
    value.g = (g1.g + g2.g + g3.g + g4.g) / 4;
    img.setValue(row, col, value);
    return;
}

/// <summary>
/// Interpolate Green and Blue on Red pixel
/// </summary>
/// <param name="img">Image for interpolation</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
void Demosaic::Bilinear::interGB(Image &img, int row, int col)
{
    Color::RGB64 value = img.getValue(row, col);

    // Diagonal surrounding (B)
    const Color::RGB64 b1 = img.getValue(row + 1, col + 1);
    const Color::RGB64 b2 = img.getValue(row - 1, col - 1);
    const Color::RGB64 b3 = img.getValue(row + 1, col - 1);
    const Color::RGB64 b4 = img.getValue(row - 1, col + 1);

    // Axis surrounding (G)
    const Color::RGB64 g1 = img.getValue(row, col + 1);
    const Color::RGB64 g2 = img.getValue(row, col - 1);
    const Color::RGB64 g3 = img.getValue(row + 1, col);
    const Color::RGB64 g4 = img.getValue(row - 1, col);

    // Interpolate
    value.g = (g1.g + g2.g + g3.g + g4.g) / 4;
    value.b = (b1.b + b2.b + b3.b + b4.b) / 4;
    img.setValue(row, col, value);
    return;
}

/// <summary>
/// Interpolate Red and Blue on Green pixel
/// </summary>
/// <param name="img">Image for interpolation</param>
/// <param name="row">Image row</param>
/// <param name="col">Image column</param>
void Demosaic::Bilinear::interRB(
    Image &img, const CFAPattern &cfa, int row, int col)
{
    Color::RGB64 value = img.getValue(row, col);

    // Axis surrounding
    const Color::RGB64 h1 = img.getValue(row, col + 1);
    const Color::RGB64 h2 = img.getValue(row, col - 1);
    const Color::RGB64 v1 = img.getValue(row + 1, col);
    const Color::RGB64 v2 = img.getValue(row - 1, col);

    // Interpolate
    if (cfa(row, col + 1) == CFAPattern::Color::BLUE)
    {
        // Blue is in horizontal values and red in vertical
        value.r = (v1.r + v2.r) / 2;
        value.b = (h1.b + h2.b) / 2;
    }
    else
    {
        // Red is in horizontal values and blue in vertical
        assert(cfa(row + 1, col) == CFAPattern::Color::BLUE);
        value.r = (h1.r + h2.r) / 2; // Vert. B
        value.b = (v1.b + v2.b) / 2;
    }
    img.setValue(row, col, value);
    return;
}
