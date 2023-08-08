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
#include "Cam5D2.hpp"
#include "Utils.hpp"

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam5D2::getCameraModel() const
{
	return "Canon EOS 5D Mark II";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam5D2::getCameraID() const
{
	return CamID::EOS_5D2;
}

/// <summary>
/// Load EOS 5D Mark II calibration info
/// </summary>
Cam5D2::Cam5D2(double temperature)
{
	// CFA pattern
	m_cfa = CFAPattern(CFAPattern::Filter::GBRG);

	// Levels
	CamProfile::m_black = { 1024, 1024, 1024 };
	CamProfile::m_white = { 15600, 15600, 15600 };
	CamProfile::m_baseExposure = 0.4;

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = { 158, 51, 5792, 3804 };

	// Camera crop 
	// CamProfile::m_crop = Rect::CreateRect(168, 56, 5616, 3744);
	// but for clean output it must be tighter...
	CamProfile::m_crop = Rect::CreateRect(168, 57, 5616, 3742);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = {{
		{ 0.8924, -0.1041,  0.1760 },
		{ 0.4351,  0.6621, -0.0972 },
		{ 0.0505, -0.1562,  0.9308 }
	}};

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam5D2::k_ColorMat1, m_illu1, Cam5D2::k_ColorMat2, m_illu2);

	// Camera calibration (only one for 5DII)
	CamProfile::m_camCalibrationMat = {{
		{ 1.0147, 0.0000, 0.0000 },
		{ 0.0000, 1.0000, 0.0000 },
		{ 0.0000, 0.0000, 0.9826 }
	}};

	// HSV profile maps
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		36, 8, 16, Cam5D2::k_ProfileLookTableData);
	return;
}

// Color matrices
const Mat3x3 Cam5D2::k_ColorMat1 = {{//@ illu1
	{  0.5309, -0.0229, -0.0336 },
	{ -0.6241,  1.3265,  0.3337 },
	{ -0.0817,  0.1215,  0.6664 }
}};
const Mat3x3 Cam5D2::k_ColorMat2 = {{//@ illu2
	{  0.4716, 0.0603, -0.0830 },
	{ -0.7798, 1.5474,  0.2480 },
	{ -0.1496, 0.1937,  0.6651 }
}};

// Profile look map data
#include "profLookMap.hpp"
