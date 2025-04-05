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

class Image;
class Options;
namespace Color { struct HSV64; };
class HSVMap;

class ProcRGBModule
{
    double m_Exposure;
    int m_Contrast;

public:
    static void run(Image &img, const Options &opt);

private:
    ProcRGBModule(const Options &opt);
    void process(Image &img, bool process);

private: // Edits
    void processImage(Image &img, bool process);
    double contrast(double value, double midpoint);
    static double levels(double value, double black,
        double gamma, double white, double outBlack, double outWhite);

private: // Curve construction
    static double basecurve(double, double, double, double, double, double);
    static double basel(double, double, double);
    static double baseu(double, double, double);
    static double cupper(double, double, double);
    static double clower(double, double, double);
    static double cupper2(double, double, double);
    static double clower2(double, double, double);
};
