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
#include "Cam5Ds.hpp"
#include "Utils.hpp"

#include <cassert>

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam5Ds::getCameraModel() const
{
	return "Canon EOS 5DS";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam5Ds::getCameraID() const
{
	return CamID::EOS_5Ds;
}

/// <summary>
/// Load EOS 5Ds calibration info
/// </summary>
Cam5Ds::Cam5Ds(double temperature)
{
	// Levels
	CamProfile::m_black = {2047, 2047, 2047};
	CamProfile::m_white = {14466, 14466, 14466};
	CamProfile::m_baseExposure = 0.17;

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = {160, 64, 8896, 5920};

	// Camera crop
	CamProfile::m_crop = Rect::CreateRect(196, 100, 8688, 5792);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = Utils::interColorMat(temperature,
		Cam5Ds::k_ForwardMat1, m_illu1, Cam5Ds::k_ForwardMat2, m_illu2
	);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam5Ds::k_ColorMat1, m_illu1, Cam5Ds::k_ColorMat2, m_illu2
	);

	// Camera calibration (only one for 5Ds)
	CamProfile::m_camCalibrationMat = {{
		{ 0.9489, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0.9687 }
	}};

	// HSV maps
	CamProfile::m_hsvMap = std::make_shared<HSVMap>(
		90, 30, 1, // Dimensions
		Cam5Ds::k_HSVMapData1, m_illu1,
		Cam5Ds::k_HSVMapData2, m_illu2, temperature);
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		36, 8, 16, Cam5Ds::k_ProfileLookTableData);
	return;
}

// Forward matrix
const Mat3x3 Cam5Ds::k_ForwardMat1 = {{//@ illu1
	{ 0.5518, 0.2528, 0.1597 },
	{ 0.3380, 0.6092, 0.0528 },
	{ 0.2019, 0.0004, 0.6229 }
}};
const Mat3x3 Cam5Ds::k_ForwardMat2 = {{//@ illu2
	{ 0.5470, 0.1929, 0.2244 },
	{ 0.3283, 0.5982, 0.0734 },
	{ 0.1679, 0.0004, 0.6567 }
}};

// Color matrix
const Mat3x3 Cam5Ds::k_ColorMat1 = {{//@ illu1
	{  0.6848, -0.1661, -0.0221 },
	{ -0.3904,  1.0931,  0.3434 },
	{ -0.0470,  0.1251,  0.6039 }
}};
const Mat3x3 Cam5Ds::k_ColorMat2 = {{//@ illu2
	{  0.6250, -0.0711, -0.0808 },
	{ -0.5153,  1.2794,  0.2636 },
	{ -0.1249,  0.2198,  0.5610 }
}};

// Camera HSV profile data
#include "hsvMap1.hpp"
#include "hsvMap2.hpp"
#include "profLookMap.hpp"
#include "toneCurve.hpp"
