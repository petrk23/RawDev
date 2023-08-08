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
#include "Cam6D2.hpp"
#include "Utils.hpp"

#include <cassert>

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam6D2::getCameraModel() const
{
	return "Canon EOS 6D Mark II";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam6D2::getCameraID() const
{
	return CamID::EOS_6D2;
}

/// <summary>
/// Load EOS 6D2 calibration info
/// </summary>
Cam6D2::Cam6D2(double temperature)
{
	// Levels
	CamProfile::m_black = { 513, 513, 513 };
	CamProfile::m_white = { 14558, 14558, 14558 };
	CamProfile::m_baseExposure = 0.25;

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = { 120, 44, 6384, 4224 };

	// Camera crop
	CamProfile::m_crop = Rect::CreateRect(132, 56, 6240, 4160);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = Utils::interColorMat(temperature,
		Cam6D2::k_ForwardMat1, m_illu1, Cam6D2::k_ForwardMat2, m_illu2
	);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam6D2::k_ColorMat1, m_illu1, Cam6D2::k_ColorMat2, m_illu2
	);

	// Camera calibration (only one for 6D2)
	CamProfile::m_camCalibrationMat = {{
		{ 0.9859, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0.9724 }
	}};

	// HSV maps
	CamProfile::m_hsvMap = std::make_shared<HSVMap>(
		90, 30, 1, // Dimensions
		Cam6D2::k_HSVMapData1, m_illu1,
		Cam6D2::k_HSVMapData2, m_illu2, temperature);
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		36, 8, 16, Cam6D2::k_ProfileLookTableData);
	return;
}

// Forward matrix
const Mat3x3 Cam6D2::k_ForwardMat1 = {{//@ illu1
	{ 0.5806, 0.2293, 0.1544 },
	{ 0.3687, 0.5807, 0.0505 },
	{ 0.2066, 0.0010, 0.6176 }
}};
const Mat3x3 Cam6D2::k_ForwardMat2 = {{//@ illu2
	{ 0.5333, 0.1999, 0.2311 },
	{ 0.3072, 0.6184, 0.0744 },
	{ 0.1433, 0.0024, 0.6794 }
}};

// Color matrix
const Mat3x3 Cam6D2::k_ColorMat1 = {{//@ illu1
	{  0.7295, -0.1664, -0.0669 },
	{ -0.4871,  1.2238,  0.2980 },
	{ -0.0585,  0.1356,  0.6696 }
}};
const Mat3x3 Cam6D2::k_ColorMat2 = {{//@ illu2
	{  0.6875, -0.0970, -0.0932 },
	{ -0.4691,  1.2459,  0.2501 },
	{ -0.0874,  0.1953,  0.5809 }
}};

// Camera HSV profile data
#include "hsvMap1.hpp"
#include "hsvMap2.hpp"
#include "profLookMap.hpp"
