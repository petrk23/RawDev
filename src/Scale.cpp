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

#include "Scale.hpp"

#include "CamProfiles/CamProfile.hpp"
#include "WhiteBalance.hpp"
#include "Structures/Image.hpp"
#include "Structures/Rect.hpp"
#include "Options.hpp"
#include "Utils.hpp"
#include "RawDev.hpp"

#include <sstream>
using namespace std;

/// <summary>
/// Scaling raw image data in camera native space
/// </summary>
/// <param name="img">Raw image</param>
/// <param name="opt">Processing options</param>
void ScaleModule::run(Image &img, const Options &opt)
{
    ScaleModule scale(img.getCamProfile(), opt);
    scale.process(img); // Eg. run main scaling
    return;
}

/// <summary>
/// Construct and load needed options
/// </summary>
/// <param name="opt">Options structure</param>
ScaleModule::ScaleModule(
    const std::shared_ptr<CamProfile> &profile, const Options &opt)
{
    m_CamProfile = profile;
    m_ColorTemp = opt.getTemperature();
    m_Tint = opt.getTint();
    return;
}

/// <summary>
/// Subtract black and scale colors
/// </summary>
/// <param name="img">Source raw image</param>
/// <param name="black">Black point</param>
/// <param name="scaleR">Red channel scale</param>
/// <param name="scaleG">Green channel scale</param>
/// <param name="scaleB">Blue channel scale</param>
void ScaleModule::scale(Image &img,
    double black, double scaleR, double scaleG, double scaleB)
{
    RawDev::verbout << "Subtracting black and scaling colors" << endl;
    const int width = img.getWidth(), height = img.getHeight();

    #pragma omp parallel for schedule(static)
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            Color::RGB64 value = img.getValue(row, col);
            value.r = (value.r - black) * scaleR;
            value.g = (value.g - black) * scaleG;
            value.b = (value.b - black) * scaleB;
            img.setValue(row, col, value); // Clips to [0-1]
        }
    }
    return;
}

/// <summary>
/// Process the image with that module
/// </summary>
/// <param name="img">Image to be processed</param>
void ScaleModule::process(Image &img)
{
    const double black = calcBlackPoint(img);

    // Baseline exposure bias
    const double baseExposure = Utils::EV2Val(m_CamProfile->getBaseExposure());
    RawDev::verbout << "Baseline exposure is ";
    printScale(nullptr, baseExposure, true);

    // Compute white balance
    const WhiteBalance wb(m_ColorTemp, m_Tint);
    const WhiteBalance::Scale wbScales = wb.calcScales(*m_CamProfile);
    RawDev::verbout << "White balance scales are";
    RawDev::verbout.newline();
    printScale("R =", wbScales.rs); // Scales on newline
    printScale("G =", wbScales.gs);
    printScale("B =", wbScales.bs, true);

    // Compute scales
    double scaleR, scaleG, scaleB;
    const Color::RGB64 white = m_CamProfile->getWhiteLevel();
    scaleR = wbScales.rs * baseExposure / (white.r - black);
    scaleG = wbScales.gs * baseExposure / (white.g - black);
    scaleB = wbScales.bs * baseExposure / (white.b - black);

    // Main scaling
    scale(img, black, scaleR, scaleG, scaleB);
    return;
}

/// <summary>
/// Calculate black level
/// </summary>
/// <param name="img">Source image</param>
/// <returns>Selected black point</returns>
double ScaleModule::calcBlackPoint(const Image &img)
{
    const Color::RGB64 rblack = m_CamProfile->getBlackLevel();
    const Color::RGB64 cblack = estimateBlackPoint(img);
    const double black = Utils::max3(cblack.r, cblack.g, cblack.b);

    RawDev::verbout // Print black point summary
        << "Reference black point "
        << rblack.r << ", " << rblack.g << ", " << rblack.b << endl
        << "Measured black point "
        << cblack.r << ", " << cblack.g << ", " << cblack.b << endl
        << "Using measured maximum black " << black << endl;
    return black;
}

/// <summary>
/// Estimate black point of image from masked pixels
/// </summary>
/// <param name="img">Source image</param>
/// <returns>Black point for every channel</returns>
Color::RGB64 ScaleModule::estimateBlackPoint(const Image &img)
{
    const Rect active = m_CamProfile->getActiveArea();
    Color::RGB64 black{ 0.0, 0.0, 0.0 }; // Black level result
    constexpr int pad = 2; // Padding of masked areas

    // Extracting black point from masked areas
    if (active.top >= 3 * pad && active.left >= 3 * pad)
    {
        Rect masked[2] = {     // Masked areas calc from active
            Rect(Point(pad, pad),
                Point(active.right - pad, active.top - pad)),
            Rect(Point(pad, active.top - pad),
                Point(active.left - pad, active.bottom - pad))
        };
        averageBlacks(img, masked, black);
    }
    else
    {
        black = img.getCamProfile()->getBlackLevel();
    }
    return black;
}

/// <summary>
/// Average black point inside masked areas
/// </summary>
/// <param name="img">Raw image</param>
/// <param name="masked">Maskded pixel rectangles</param>
/// <param name="black">Resulting black point</param>
void ScaleModule::averageBlacks(
    const Image &img, Rect(&masked)[2], Color::RGB64 &black)
{
    const CFAPattern cfa = m_CamProfile->getCFAPattern();
    int rcount = 1, gcount = 1, bcount = 1;

    for (int m = 0; m < 2; m++)
    {
        const int right = masked[m].right,
            bottom = masked[m].bottom, left = masked[m].left;

        for (int row = masked[m].top; row < bottom; row++)
        {
            for (int col = left; col < right; col++)
            {
                double value; // Var for loading channel values

                switch (cfa(row, col))
                {
                case CFAPattern::Color::RED:
                    value = img.getValueR(row, col);
                    black.r = Utils::incAverage(black.r, value, rcount++);
                    break;
                case CFAPattern::Color::BLUE:
                    value = img.getValueB(row, col);
                    black.b = Utils::incAverage(black.b, value, bcount++);
                    break;
                default: // GREEN_R || GREEN_B
                    value = img.getValueG(row, col);
                    black.g = Utils::incAverage(black.g, value, gcount++);
                    break;
                }
            } // col loop
        } // row loop
    }
    return;
}

/// <summary>
/// Printing of scaling factors with EV value
/// </summary>
/// <param name="value">Scale value</param>
/// <param name="last">Is last element to print</param>
void ScaleModule::printScale(
    const char* name, const double value, const bool last)
{
    std::stringstream ss;

    // Print name of the scale
    if (name != nullptr)
        ss << name << " ";

    // Print with specified precision
    const streamsize prec = ss.precision();
    ss << value << " is ";
    ss.precision(2);
    ss << fixed << Utils::val2EV(value) << "EV";
    ss.unsetf(ios_base::fixed);  // Return from fixed
    ss.precision(prec);
    RawDev::verbout << ss.str(); // Print out to verbose

    // List format
    if (!last)
        RawDev::verbout << ", ";
    else
        RawDev::verbout << endl;
    return;
}
