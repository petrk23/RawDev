/*
 * This file is part of RawDev.
 *
 * Copyright (C) 2020-2023 Petr Krajník
 *
 * RawDev is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RawDev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RawDev.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "Output.hpp"

#include "Structures/Image.hpp"
#include "Options.hpp"
#include "RawDev.hpp"

#include "CamProfiles/CamProfile.hpp"
#include "Structures/Mat3x3.hpp"
#include "Structures/LUT.hpp"
#include "ImageIO/TiffWriter.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

#include <iostream>
#include <sstream>
#include <ctime>

using namespace std;

/// <summary>
/// Module construction
/// </summary>
/// <param name="opt">Processing options</param>
OutputModule::OutputModule(const Options &opt)
{
	m_InputFile = opt.getInputFile();
	m_OutputFile = opt.getOutputFile();
	return;
}

/// <summary>
/// Run output module
/// </summary>
/// <param name="img">Image to be processed</param>
/// <param name="opt">Processing options</param>
void OutputModule::run(Image &img, const Options &opt)
{
	OutputModule output(opt);
	try
	{
		output.process(img, opt);
	}
	catch (const Exception& ex)
	{
		cerr << "Failed to write the output file!" << endl;
		cerr << ex.what() << endl;
	}
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
void OutputModule::process(Image &img, const Options& opt)
{
	const int bits = opt.getBitDepth();
	const ColorProfile colorProfile = opt.getColorProfile();

	// Convert ProPhoto to target profile
	if (colorProfile == ColorProfile::aRGB)
	{
		conversionMessage("AdobeRGB(1998)", "2.2");
		convert2argb(img);
	}
	else if (colorProfile == ColorProfile::sRGB)
	{
		conversionMessage("sRGB", "curve");
		convert2srgb(img);
	}

	// Writing the resulting output
	RawDev::verbout
		<< "Writing output to '" << m_OutputFile << "' ("
		<< opt.getBitDepth() << "bits)" << endl;
	TiffWriter tw(m_OutputFile, bits, opt.getNoCrop()); // Write as TIFF
	tw.setDocumentName(m_InputFile.getFileName());
	tw.setICC(opt.getColorProfile());
	tw.setMake("Canon");
	tw.setModel(img.getCamProfile()->getCameraModel());
	tw.setArtist(m_Artist);
	tw.setCopyright(formatCopyright());
	tw.write(img);
	return;
}

/// <summary>
/// Format copyright notice
/// </summary>
/// <returns>Copyright sentence</returns>
std::string OutputModule::formatCopyright() const
{
	if (m_Artist.empty() == false)
	{
		stringstream ss;
		ss << "Copyright (C)" << getYear() << " by " << m_Artist;
		return ss.str();
	}
	return m_Artist;
}

/// <summary>
/// Get current year
/// </summary>
/// <returns>Year as a integer</returns>
int OutputModule::getYear()
{
	time_t t = time(NULL);
	tm time; // Get time from OS
	Utils::localTime(&t, &time);
	return time.tm_year + 1900;
}

/// <summary>
/// Convert working ProPhoto to aRGB (precise)
/// </summary>
/// <param name="img">Image to be processed</param>
void OutputModule::convert2argb(Image& img)
{
	const Mat3x3 ProPhoto2ARGB =
		Color::XYZtoARGB * Color::XYZtoProPhotoRGB.inverse();
	constexpr double invgamma = 1 / 2.2;
	const int width = img.getWidth(), height = img.getHeight();

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			Color::RGB64 value = img.getValue(i, j);

			// Convert to ProPhoto -> XYZ -> AdobeRGB
			value = ProPhoto2ARGB * value;

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
		Color::XYZtoSRGB * Color::XYZtoProPhotoRGB.inverse();
	const int width = img.getWidth(), height = img.getHeight();

	#pragma omp	parallel for schedule(static)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			Color::RGB64 value = img.getValue(i, j);

			// Convert to ProPhoto -> XYZ -> sRGB
			value = ProPhoto2SRGB * value;

			// Gamma curve for sRGB
			value.r = srgbGammaCurve(value.r);
			value.g = srgbGammaCurve(value.g);
			value.b = srgbGammaCurve(value.b);
			img.setValue(i, j, value);
		}
	}
	return;
}

/// <summary>
/// Gamma correction with LUT (approximate)
/// </summary>
/// <param name="img">Image to be processed</param>
/// <param name="gamma">Gamma function value</param>
void OutputModule::gammaCorrectionLUT(Image &img, const double gamma)
{
	const Mat3x3 ProPhoto2ARGB =
		Color::XYZtoARGB * Color::XYZtoProPhotoRGB.inverse();
	const double invgamma = 1 / (gamma);
	const int width = img.getWidth(), height = img.getHeight();

	// Initialize LUT
	LUTd gammaLUT(65536);
	double g = 0.0, step = 1.0 / 65536.0;
	for (int c = 0; c < 65536; c++)
	{
		gammaLUT[c] = pow(g, invgamma);
		g += step;
	}

	#pragma omp parallel for schedule(static)
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			Color::RGB64 value = img.getValue(i, j);

			// Convert to ProPhoto -> XYZ -> AdobeRGB
			value = ProPhoto2ARGB * value;

			// Gamma 2.2 for Adobe RGB
			value.r = gammaLUT[Image::clipDouble(value.r)];
			value.g = gammaLUT[Image::clipDouble(value.g)];
			value.b = gammaLUT[Image::clipDouble(value.b)];
			img.setValue(i, j, value);
		}
	}
	return;
}
