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
#include "Cam6D.hpp"
#include "Utils.hpp"

#include <cassert>

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam6D::getCameraModel() const
{
	return "Canon EOS 6D";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam6D::getCameraID() const
{
	return CamID::EOS_6D;
}

/// <summary>
/// Load EOS 6D calibration info
/// </summary>
Cam6D::Cam6D(double temperature)
{
	// Levels
	CamProfile::m_black = { 2047, 2047, 2047 };
	CamProfile::m_white = { 15000, 15000, 15000 };
	CamProfile::m_baseExposure = 0.25;

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = { 72, 38, 5568, 3708 };

	// Camera crop
	CamProfile::m_crop = Rect::CreateRect(84, 50, 5472, 3648);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = Utils::interColorMat(temperature,
		Cam6D::k_ForwardMat1, m_illu1, Cam6D::k_ForwardMat2, m_illu2
	);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam6D::k_ColorMat1, m_illu1, Cam6D::k_ColorMat2, m_illu2
	);

	// Camera calibration (only one for 6D)
	CamProfile::m_camCalibrationMat = {{
		{ 1.0135, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0.9982 }
	}};

	// HSV maps
	CamProfile::m_hsvMap = std::make_shared<HSVMap>(
		90, 30, 1, // Dimensions
		Cam6D::k_HSVMapData1, m_illu1, 
		Cam6D::k_HSVMapData2, m_illu2, temperature);
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		36, 8, 16, Cam6D::k_ProfileLookTableData);
	return;
}

// Forward matrix
const Mat3x3 Cam6D::k_ForwardMat1 = {{//@ illu1
	{  0.7763,  0.0065,  0.1815 },
	{  0.2364,  0.8351, -0.0715 },
	{ -0.0059, -0.4228,  1.2538 }
}};
const Mat3x3 Cam6D::k_ForwardMat2 = {{//@ illu2
	{ 0.7464,  0.1044,  0.1135 },
	{ 0.2648,  0.9173, -0.1820 },
	{ 0.0113, -0.2154,  1.0292 }
}};

// Color matrix
const Mat3x3 Cam6D::k_ColorMat1 = {{//@ illu1
	{  0.7546, -0.1435, -0.0929 },
	{ -0.3846,  1.1488,  0.2692 },
	{ -0.0332,  0.1209,  0.6370 }
}};
const Mat3x3 Cam6D::k_ColorMat2 = {{//@ illu2
	{  0.7034, -0.0804, -0.1014 },
	{ -0.4420,  1.2564,  0.2058 },
	{ -0.0851,  0.1994,  0.5758 }
}};

// Camera HSV profile data
#include "hsvMap1.hpp"
#include "hsvMap2.hpp"
#include "profLookMap.hpp"
