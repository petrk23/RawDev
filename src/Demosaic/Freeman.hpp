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
#include "Structures/Rect.hpp"

template<typename T> class Array2D;
class Logger;

namespace Demosaic
{
    class Freeman : public IAlgorithm
    {
        Rect m_ActiveArea;
        int m_MedianIter;

    public: // IAlgorithm interface
        Freeman(int medianIter);
        virtual ~Freeman();
        virtual void demosaic(Image &img);
        virtual void printLogo(Logger &os) const;

    private: // Helpers
        void calcChannelDiff(const Image &img,
            Array2D<double> &diffRG, Array2D<double> &diffBG);
        void calcImageFromDiff(Image &img,
            const Array2D<double> &diffRG, const Array2D<double> &diffBG);
        static void median(Array2D<double> &channel);
    };
};

////////////////////////////////////////////////////////////////////////////////

inline Demosaic::Freeman::Freeman(int medianIter)
    : m_ActiveArea(), m_MedianIter(medianIter)
{}
