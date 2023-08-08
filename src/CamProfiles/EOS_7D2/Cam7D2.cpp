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
#include "Cam7D2.hpp"
#include "Utils.hpp"

#include <cassert>

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam7D2::getCameraModel() const
{
	return "Canon EOS 7D Mark II";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam7D2::getCameraID() const
{
	return CamID::EOS_7D2;
}

/// <summary>
/// Load EOS 7D2 calibration info
/// </summary>
Cam7D2::Cam7D2(double temperature)
{
	// Levels
	CamProfile::m_black = { 2047, 2047, 2047 };
	CamProfile::m_white = { 13400, 13400, 13400 };
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
		Cam7D2::k_ForwardMat1, m_illu1, Cam7D2::k_ForwardMat2, m_illu2
	);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam7D2::k_ColorMat1, m_illu1, Cam7D2::k_ColorMat2, m_illu2
	);

	// Camera calibration (only one for 7D2)
	CamProfile::m_camCalibrationMat = {{
		{ 1.0115, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0.9921 }
	}};

	// HSV maps
	CamProfile::m_hsvMap = std::make_shared<HSVMap>(
		90, 30, 1, // Dimensions
		Cam7D2::k_HSVMapData1, m_illu1,
		Cam7D2::k_HSVMapData2, m_illu2, temperature);
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		36, 8, 16, Cam7D2::k_ProfileLookTableData);
	return;
}

// Forward matrix
const Mat3x3 Cam7D2::k_ForwardMat1 = {{//@ illu1
	{ 0.5165, 0.2766, 0.1712 },
	{ 0.2991, 0.6466, 0.0544 },
	{ 0.1694, 0.0011, 0.6546 }
}};
const Mat3x3 Cam7D2::k_ForwardMat2 = {{//@ illu2
	{ 0.5160, 0.2078, 0.2405 },
	{ 0.3024, 0.6191, 0.0786 },
	{ 0.1525, 0.0026, 0.6700 }
}};

// Color matrix
const Mat3x3 Cam7D2::k_ColorMat1 = {{//@ illu1
	{  0.7795, -0.2196, -0.0251 },
	{ -0.3391,  1.0273,  0.3632 },
	{ -0.0373,  0.1283,  0.6613 }
}};
const Mat3x3 Cam7D2::k_ColorMat2 = {{//@ illu2
	{  0.7268, -0.1082, -0.0969 },
	{ -0.4186,  1.1839,  0.2663 },
	{ -0.0825,  0.2029,  0.5839 }
}};

// Camera HSV profile data
#include "hsvMap1.hpp"
#include "hsvMap2.hpp"
#include "profLookMap.hpp"
