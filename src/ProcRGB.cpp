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

#include "ProcRGB.hpp"

#include "Structures/Image.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "Options.hpp"
#include "Utils.hpp"
#include "RawDev.hpp"

#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

/// <summary>
/// Module construction
/// </summary>
/// <param name="opt">Processing options</param>
ProcRGBModule::ProcRGBModule(const Options &opt)
{
    m_Exposure = opt.getExposure();
    m_Contrast = opt.getContrast();
    return;
}

/// <summary>
/// Run output module
/// </summary>
/// <param name="img">Image to be processed</param>
/// <param name="opt">Processing options</param>
void ProcRGBModule::run(Image &img, const Options &opt)
{
    ProcRGBModule procrgb(opt);
    bool process = !opt.getNoProcess();
    procrgb.process(img, process);
    return;
}

/// <summary>
/// Process image in linear ProPhotoRGB working space.
/// Camera profile is also applied in this step.
/// </summary>
/// <param name="img">Image to be processed</param>
void ProcRGBModule::process(Image &img, bool process)
{
    // Print what is to be done
    RawDev::verbout
        << "Convert from camera native to working color space";
    RawDev::verbout.newline();
    RawDev::verbout
        << "Working color space: Linear ProPhoto RGB" << endl
        << "Apply camera profile look table (HSV)" << endl;
    if (process)
        RawDev::verbout << "Apply processing curves" << endl;

    // Main process image
    processImage(img, process);
    return;
}

/// <summary>
/// Process RGB image
/// </summary>
/// <param name="img">Image to be processed</param>
void ProcRGBModule::processImage(Image &img, bool process)
{
    const shared_ptr<CamProfile> profile = img.getCamProfile();
    const Mat3x3 cam2work = Color::k_matXYZtoProPhotoRGB
        * profile->getForwardMatrix()
        * profile->getAnalogBalanceMatrix().inverse();
    const int width = img.getWidth(), height = img.getHeight();
    const double middleGray = pow(0.5, 2.2), // Gamma 2.2
        expcomp = Utils::EV2Val(1 + m_Exposure), // +1 to match ACR
        recovery = 1.0 - 1.0 / expcomp; // Hightlights recovery
    Color::RGB64 v; // Pixel values
    Color::HSV64 hsv;

    #pragma omp parallel for schedule(static) private(v, hsv)
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // Convert from camera to working color space
            v = Color::rgbTo<Color::RGB64>(cam2work, img.getValue(row, col));
            v.r = std::clamp(v.r, 0.0, 1.0);
            v.g = std::clamp(v.g, 0.0, 1.0);
            v.b = std::clamp(v.b, 0.0, 1.0);

            // HSV profile processing
            if (profile->hasHSVMaps() == true)
            {
                hsv = Color::rgb2hsv(v);
                profile->applyHSVMap(hsv);
                profile->applyProfileLook(hsv);
                v = Color::hsv2rgb(hsv); // Convert back to RGB
            }

            // Process edits
            if (process)
            {
                // Curves maping (the main mapping)
                v.r = basecurve(v.r, expcomp, 0.0, 1.0, recovery, 0.0);
                v.g = basecurve(v.g, expcomp, 0.0, 1.0, recovery, 0.0);
                v.b = basecurve(v.b, expcomp, 0.0, 1.0, recovery, 0.0);

                // Contrast S-Curve
                if (m_Contrast != 0)
                {
                    v.r = contrast(v.r, middleGray);
                    v.g = contrast(v.g, middleGray);
                    v.b = contrast(v.b, middleGray);
                }
                /* Other edits : TODO */
            }
            img.setValue(row, col, v); // Save the result
        }
    }
    return;
}

/// <summary>
/// Apply S-Curve to a value
/// </summary>
/// <param name="value">Value to be mapped</param>
/// <param name="midpoint">Data neutral gray</param>
/// <returns>Mapped value</returns>
double ProcRGBModule::contrast(double value, double midpoint)
{
    assert(m_Contrast >= -100 && m_Contrast <= 100);
    const double g = 1 / (1 - m_Contrast * 0.009);

    if (value <= 0.0)
        return 0.0;
    else if (value > 1.0)
        return 1.0;
    else if (value < midpoint)
        value = midpoint * pow(value / midpoint, g);
    else
        value = 1 - (1 - midpoint) * pow((1 - value) / (1 - midpoint), g);
    return value;
}

/// <summary>
/// Apply level adjustment as in photoshop
/// </summary>
/// <param name="value">Value to be processed</param>
/// <param name="black">Black point</param>
/// <param name="gamma">Gamma value (middle tones)</param>
/// <param name="white">White point</param>
/// <param name="outBlack">Ouptut black</param>
/// <param name="outWhite">Output white</param>
/// <returns>Processed value</returns>
double ProcRGBModule::levels(
    double value,
    double black, double gamma, double white,
    double outBlack, double outWhite)
{
    value = outBlack
        + (value - black) * (outWhite - outBlack) / (white - black);
    return pow(value, gamma);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*
Basic tone curve borrowed from RawTherapee (rtengine/curves.h)
TODO: Create my own cruve generator that is better
*/

/*
Base curve for processing
*/
double ProcRGBModule::basecurve(
    double val, double exposure, double black, double white, double hr, double sr)
{
    if (black < 0) {
        double m = 0.5; // Midpoint
        double slope = 1.0 + black; // Slope of straight line between (0,-black) and (1,1)
        double y = -black + m * slope; // Value at midpoint

        if (val > m) {
            // Value on straight line between (m,y) and (1,1)
            return y + (val - m) * slope;
        }
        else {
            return y * clower2(val / m, slope * m / y, 2.0 - sr);
        }
    }
    else {
        double slope = exposure / (1.0 - black);
        double m = exposure * white > 1.0
            ? black / exposure + (0.25) / slope : black + (1 - black) / 4;
        double y = exposure * white > 1.0
            ? 0.25 : (m - black / exposure) * slope;

        if (val <= m) {
            return black == 0
                ? val * slope : clower(val / m, slope * m / y, sr) * y;
        }
        else if (exposure * white > 1.0) {
            return y + (1.0 - y) * cupper2(
                (val - m) / (white - m), slope * (white - m) / (1.0 - y), hr);
        }
        else {
            return y + (val - m) * slope;
        }
    }
}

/*
Basic convex function between (0,0) and (1,1).
m1 and m2 controls the slope at the start and end point
*/
inline double ProcRGBModule::basel(double x, double m1, double m2)
{
    if (x == 0.0) {
        return 0.0;
    }

    double k = sqrt((m1 - 1.0) * (m1 - m2) * 0.5) / (1.0 - m2);
    double l = (m1 - m2) / (1.0 - m2) + k;
    double lx = log(x);
    return m2 * x + (1.0 - m2) * (2.0 - exp(k * lx)) * exp(l * lx);
}


/*
Basic concave function between (0,0) and (1,1).
m1 and m2 controls the slope at the start and end point
*/
inline double ProcRGBModule::baseu(double x, double m1, double m2)
{
    return 1.0 - basel(1.0 - x, m1, m2);
}

/*
Convex curve between (0,0) and (1,1) with slope m at (0,0).
hr controls the highlight recovery
*/
inline double ProcRGBModule::cupper(double x, double m, double hr)
{
    if (hr > 1.0) {
        return baseu(x, m, 2.0 * (hr - 1.0) / m);
    }

    double x1 = (1.0 - hr) / m;
    double x2 = x1 + hr;

    if (x >= x2) {
        return 1.0;
    }

    if (x < x1) {
        return x * m;
    }

    return 1.0 - hr + hr * baseu((x - x1) / hr, m, 0);
}

/*
Concave curve between (0,0) and (1,1) with slope m at (1,1).
sr controls the shadow recovery
*/
inline double ProcRGBModule::clower(double x, double m, double sr)
{
    return 1.0 - cupper(1.0 - x, m, sr);
}

/*
Convex curve between (0,0) and (1,1) with slope m at (0,0).
hr controls the highlight recovery
*/
inline double ProcRGBModule::cupper2(double x, double m, double hr)
{
    double x1 = (1.0 - hr) / m;
    double x2 = x1 + hr;

    if (x >= x2) {
        return 1.0;
    }

    if (x < x1) {
        return x * m;
    }

    return 1.0 - hr + hr * baseu((x - x1) / hr, m, 0.3 * hr);
}

/*
Concave curve between (0,0) and (1,1) with slope m at (0,0).
sr controls the shadow recovery
*/
inline double ProcRGBModule::clower2(double x, double m, double sr)
{
    //curve for b<0; starts with positive slope and then rolls over toward straight line to x=y=1
    double x1 = sr / 1.5 + 0.00001;

    if (x > x1 || sr < 0.001) {
        return 1 - (1 - x) * m;
    }
    else {
        double y1 = 1 - (1 - x1) * m;
        return y1 + m * (x - x1) - (1 - m) * Utils::sqr(Utils::sqr(1 - x / x1));
    }
}
