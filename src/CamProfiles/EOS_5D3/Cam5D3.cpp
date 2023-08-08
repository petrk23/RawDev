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
#include "Cam5D3.hpp"
#include "Utils.hpp"

#include <cassert>

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam5D3::getCameraModel() const
{
	return "Canon EOS 5D Mark III";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam5D3::getCameraID() const
{
	return CamID::EOS_5D3;
}

/// <summary>
/// Load EOS 5D3 calibration info
/// </summary>
Cam5D3::Cam5D3(double temperature)
{
	// Levels
	CamProfile::m_black = { 2047, 2047, 2047 };
	CamProfile::m_white = { 15000, 15000, 15000 };
	CamProfile::m_baseExposure = 0.25;

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = { 122, 80, 5918, 3950 };

	// Camera crop
	CamProfile::m_crop = Rect::CreateRect(140, 96, 5760, 3840);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = Utils::interColorMat(temperature,
		Cam5D3::k_ForwardMat1, m_illu1, Cam5D3::k_ForwardMat2, m_illu2
	);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam5D3::k_ColorMat1, m_illu1, Cam5D3::k_ColorMat2, m_illu2
	);

	// Camera calibration (only one for 5D3)
	CamProfile::m_camCalibrationMat = {{
		{ 1.0138, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0.9784 }
	}};

	// HSV maps
	CamProfile::m_hsvMap = std::make_shared<HSVMap>(
		90, 30, 1, // Dimensions
		Cam5D3::k_HSVMapData1, m_illu1,
		Cam5D3::k_HSVMapData2, m_illu2, temperature);
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		36, 8, 16, Cam5D3::k_ProfileLookTableData);
	return;
}

// Forward matrix
const Mat3x3 Cam5D3::k_ForwardMat1 = {{//@ illu1
	{ 0.7868,  0.0092,  0.1683 },
	{ 0.2291,  0.8615, -0.0906 },
	{ 0.0027, -0.4752,  1.2976 }
}};
const Mat3x3 Cam5D3::k_ForwardMat2 = {{//@ illu2
	{ 0.7637,  0.0805,  0.1201 },
	{ 0.2649,  0.9179, -0.1828 },
	{ 0.0137, -0.2456,  1.0570 }
}};

// Color matrix
const Mat3x3 Cam5D3::k_ColorMat1 = {{//@ illu1
	{  0.7234, -0.1413, -0.0600 },
	{ -0.3631,  1.1150,  0.2850 },
	{ -0.0382,  0.1335,  0.6437 }
}};
const Mat3x3 Cam5D3::k_ColorMat2 = {{//@ illu2
	{  0.6722, -0.0635, -0.0963 },
	{ -0.4287,  1.2460,  0.2028 },
	{ -0.0908,  0.2162,  0.5668 }
}};

// Camera HSV profile data
#include "hsvMap1.hpp"
#include "hsvMap2.hpp"
#include "profLookMap.hpp"
