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

#include "Output.hpp"

#include "ArtistNameValidator.hpp"
#include "CamProfiles/CamProfile.hpp"
#include "ImageIO/TiffWriter.hpp"
#include "Options.hpp"
#include "RawDev.hpp"
#include "Structures/Image.hpp"
#include "Structures/Mat3x3.hpp"
#include "TimeUtils.hpp"

#include <sstream>

using namespace std;

/// <summary>
/// Module construction
/// </summary>
/// <param name="opt">Processing options</param>
OutputModule::OutputModule(const Options& opt)
{
    m_InputFile = opt.getInputFile();
    m_OutputFile = opt.getOutputFile();

    m_Artist = opt.getArtistName();
#ifndef NDEBUG
    ArtistNameValidator artistNameValidator;
    artistNameValidator(m_Artist);

    // Check only for correctness in debug mode
    // as this should be already checked when
    // parsing options.
#endif
}

/// <summary>
/// Run output module
/// </summary>
/// <param name="img">Image to be processed</param>
/// <param name="opt">Processing options</param>
void OutputModule::run(Image& img, const Options& opt)
{
    OutputModule output(opt);
    output.process(img, opt);
    return;
}

/// <summary>
/// Print profile conversion message
/// </summary>
/// <param name="profileName">Profile name string</param>
/// <param name="curveName">Gamma curve name or spec.</param>
void OutputModule::conversionMessage(const char* profileName, const char* curveName)
{
    RawDev::verbout
        << "Converting from ProPhoto to " << profileName << endl
        << "Gamma correction " << curveName << " for " << profileName << endl;
    return;
}

/// <summary>
/// Process image with output module
/// </summary>
/// <param name="img">Image to be processed</param>
void OutputModule::process(Image& img, const Options& opt)
{
    const int bits = opt.getBitDepth();
    const ColorProfile colorProfile = opt.getColorProfile();

    // Convert ProPhoto to target profile
    if (colorProfile == ColorProfile::aRGB) {
        conversionMessage("AdobeRGB(1998)", "2.2");
        convert2argb(img);
    }
    else if (colorProfile == ColorProfile::sRGB) {
        conversionMessage("sRGB", "curve");
        convert2srgb(img);
    }

    // Writing the resulting output
    RawDev::verbout
        << "Writing output to '" << m_OutputFile << "' ("
        << opt.getBitDepth() << "bits)" << endl;

    // Write the image as TIFF
    TiffWriter tw(m_OutputFile, bits, opt.getNoCrop());
    tw.setDocumentName(m_InputFile.getFileName());
    tw.setICC(opt.getColorProfile());
    tw.setMake("Canon");
    tw.setModel(std::string(img.getCamProfile()->getCameraName()));
    tw.setArtist(m_Artist);
    tw.setCopyright(formatCopyright());
    tw.write(img);
}

/// <summary>
/// Format copyright notice
/// </summary>
/// <returns>Copyright sentence</returns>
std::string OutputModule::formatCopyright() const
{
    if (!m_Artist.empty()) {
        stringstream ss;
        ss << "Copyright (C) " << TimeUtils::localYear() << " by " << m_Artist;
        return ss.str();
    }
    return m_Artist;
}

/// <summary>
/// Convert working ProPhoto to aRGB (precise)
/// </summary>
/// <param name="img">Image to be processed</param>
void OutputModule::convert2argb(Image& img)
{
    const Mat3x3 ProPhoto2ARGB =
        Color::k_matXYZtoARGB * Color::k_matXYZtoProPhotoRGB.inverse();
    constexpr double invgamma = 1 / 2.2;
    const int width = img.getWidth(), height = img.getHeight();

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            auto value = // Convert to ProPhoto -> XYZ -> AdobeRGB
                Color::rgbTo<Color::RGB64>(ProPhoto2ARGB, img.getValue(i, j));

            // Gamma 2.2 for Adobe RGB
            value.r = pow(value.r, invgamma);
            value.g = pow(value.g, invgamma);
            value.b = pow(value.b, invgamma);
            img.setValue(i, j, value);
        }
    }
    return;
}

/// <summary>
/// Special characteristic sRGB gamma curve
/// </summary>
/// <param name="value">Linear value to be converted</param>
/// <returns>Converted nonlinear value</returns>
double OutputModule::srgbGammaCurve(double value)
{
    double result;

    if (value <= 0.0031308)
        result = 12.92 * value;
    else
        result = 1.055 * pow(value, 1 / 2.4) - 0.055;
    return result;
}

/// <summary>
/// Convert working ProPhoto to sRGB (precise)
/// </summary>
/// <param name="img">Image to be processed</param>
void OutputModule::convert2srgb(Image& img)
{
    const Mat3x3 ProPhoto2SRGB =
        Color::k_matXYZtoSRGB * Color::k_matXYZtoProPhotoRGB.inverse();
    const int width = img.getWidth(), height = img.getHeight();

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            auto value = // Convert to ProPhoto -> XYZ -> sRGB
                Color::rgbTo<Color::RGB64>(ProPhoto2SRGB, img.getValue(i, j));

            // Gamma curve for sRGB
            value.r = srgbGammaCurve(value.r);
            value.g = srgbGammaCurve(value.g);
            value.b = srgbGammaCurve(value.b);
            img.setValue(i, j, value);
        }
    }
    return;
}
