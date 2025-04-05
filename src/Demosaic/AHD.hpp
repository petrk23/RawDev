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

#include "Demosaic/Algorithm.hpp"
#include "Structures/Image.hpp"

template<typename T> class Array2D;
namespace Color { struct RGB64; struct CIELAB; };
struct CFAPattern;
struct Mat3x3;
class Logger;

namespace Demosaic
{
    class AHD : public IAlgorithm
    {
        /// <summary>
        /// Data type of homogeneity maps
        /// </summary>
        typedef unsigned char homo_t;

        /// <summary>
        /// Tile size for parallel interpolation
        /// </summary>
        static constexpr int xTileSize = 512, yTileSize = 512;

    public: // IAlgorithm interface
        virtual ~AHD();
        virtual void demosaic(Image &img);
        virtual void printLogo(Logger &os) const;

    private: // Tiling helpers
        int calcTileCount(int dim, int ts);

    private: // AHD algorithm functions
        void interGreen(
            const Image &img, const CFAPattern &cfa, int brow, int bcol,
            Array2D<Color::RGB64> &himg, Array2D<Color::RGB64> &vimg);
        int initRowInfo(
            const CFAPattern &cfa, int brow, int bcol, Image::Channel &ctype);
        void interRedBlue(
            const Image &img, const CFAPattern &cfa, int brow, int bcol,
            Array2D<Color::RGB64> &timg, Array2D<Color::CIELab> &imgLab,
            const Mat3x3 &cam2XYZ);
        void generateHomogenityMasks(
            const Image &img, const int brow, const int bcol,
            const Array2D<Color::CIELab> &himgLab, Array2D<homo_t> &hhomo,
            const Array2D<Color::CIELab> &vimgLab, Array2D<homo_t> &vhomo);
        void composeOutput(
            Image &img, const int brow, const int bcol,
            const Array2D<Color::RGB64> &himg, const Array2D<homo_t> &hhomo,
            const Array2D<Color::RGB64> &vimg, const Array2D<homo_t> &vhomo);

    private: // Interpolation helpers
        Color::RGB64 interOnGreenR(
            const Image &img, const int row, const int col,
            const Array2D<Color::RGB64> &tileimg, const int tr, const int tc);
        Color::RGB64 interOnGreenB(
            const Image &img, const int row, const int col,
            const Array2D<Color::RGB64> &tileimg, const int tr, const int tc);
        Color::RGB64 interOnRed(
            const Image &img, const int row, const int col,
            const Array2D<Color::RGB64> &tileimg, const int tr, const int tc);
        Color::RGB64 interOnBlue(
            const Image& img, const int row, const int col,
            const Array2D<Color::RGB64>& tileimg, const int tr, const int tc);

    private: // Homogeneity build helpers
        void averageHomogenity(
            const Array2D<homo_t> &hhomo, const Array2D<homo_t> &vhomo,
            int row, int col, homo_t &hhm, homo_t&vhm);
        Color::CIELab camRGB2Lab(const Mat3x3 &cam2XYZ, const Color::RGB64 &src);
    };
};
