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
#include "Cam5D4.hpp"
#include "Utils.hpp"

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam5D4::getCameraModel() const
{
	return "Canon EOS 5D Mark IV";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam5D4::getCameraID() const
{
	return CamID::EOS_5D4;
}

/// <summary>
/// Load EOS 5D Mark IV calibration info
/// </summary>
Cam5D4::Cam5D4(double temperature)
{
	// Levels
	CamProfile::m_black = { 512, 512, 512 };
	CamProfile::m_white = { 14008, 14008, 14008 };
	CamProfile::m_baseExposure = 0.13;

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = { 136, 42, 6880, 4544 };

	// Camera crop
	CamProfile::m_crop = Rect::CreateRect(148, 54, 6720, 4480);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = Utils::interColorMat(temperature,
		Cam5D4::k_ForwardMat1, m_illu1, Cam5D4::k_ForwardMat2, m_illu2
	);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam5D4::k_ColorMat1, m_illu1, Cam5D4::k_ColorMat2, m_illu2
	);

	// Camera calibration (only one for 5DIV)
	CamProfile::m_camCalibrationMat = {{
		{ 1.0048, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0.9722 }
	}};

	// HSV maps
	CamProfile::m_hsvMap = std::make_shared<HSVMap>(
		90, 30, 1, // Dimensions
		Cam5D4::k_HSVMapData1, m_illu1,
		Cam5D4::k_HSVMapData2, m_illu2, temperature);
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		30, 8, 16, Cam5D4::k_ProfileLookTableData);
	return;
}

// Forward matrix
const Mat3x3 Cam5D4::k_ForwardMat1 = {{//@illu1
	{ 0.5716, 0.2264, 0.1663 },
	{ 0.3791, 0.5665, 0.0544 },
	{ 0.2297, 0.0006, 0.5948 }
}};
const Mat3x3 Cam5D4::k_ForwardMat2 = {{//@illu2
	{ 0.5497, 0.1714, 0.2433 },
	{ 0.3179, 0.6022, 0.0799 },
	{ 0.1484, 0.0025, 0.6742 }
}};

// Color matrix
const Mat3x3 Cam5D4::k_ColorMat1 = {{//@illu1
	{  0.6515, -0.0927, -0.0634 },
	{ -0.4068,  1.1151,  0.3359 },
	{ -0.0641,  0.1839,  0.7328 }
}};
const Mat3x3 Cam5D4::k_ColorMat2 = {{//@illu2
	{  0.6446, -0.0366, -0.0864 },
	{ -0.4436,  1.2204,  0.2513 },
	{ -0.0952,  0.2496,  0.6348 }
}};

// HSV maps
#include "hsvMap1.hpp"
#include "hsvMap2.hpp"
#include "profLookMap.hpp"
