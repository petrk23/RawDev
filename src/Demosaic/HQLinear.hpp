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
class Logger;

namespace Demosaic
{
    class HQLinear : public IAlgorithm
    {
    public: // IAlgorithm interface
        virtual ~HQLinear();
        virtual void demosaic(Image &img);
        virtual void printLogo(Logger &os) const;

    private: // Nine demosaic patterns
        static double interGreenFromRed(const Image &img, int row, int col);
        static double interGreenFromBlue(const Image &img, int row, int col);
        static double interRedFromGreenR(const Image &img, int row, int col);
        static double interRedFromGreenB(const Image &img, int row, int col);
        static double interRedFromBlue(const Image &img, int row, int col);
        static double interBlueFromGreenB(const Image &img, int row, int col);
        static double interBlueFromGreenR(const Image &img, int row, int col);
        static double interBlueFromRed(const Image &img, int row, int col);
    };
};
