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
#include "Cam7D.hpp"
#include "Utils.hpp"

#include <cassert>

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam7D::getCameraModel() const
{
	return "Canon EOS 7D";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam7D::getCameraID() const
{
	return CamID::EOS_7D;
}

/// <summary>
/// Load EOS 7D calibration info
/// </summary>
Cam7D::Cam7D(double temperature)
{
	// CFA
	CamProfile::m_cfa = CFAPattern::Filter::GBRG;

	// Levels
	CamProfile::m_black = { 2047, 2047, 2047 };
	CamProfile::m_white = { 13600, 13600, 13600 };
	CamProfile::m_baseExposure = -0.21;

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = { 158, 51, 5360, 3516 };

	// Camera crop
	// CamProfile::m_crop = Rect::CreateRect(168, 56, 5184, 3456);
	// but for clean output it must be tighter...
	CamProfile::m_crop = Rect::CreateRect(168, 58, 5184, 3452);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = Utils::interColorMat(temperature,
		Cam7D::k_ForwardMat1, m_illu1, Cam7D::k_ForwardMat2, m_illu2
	);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam7D::k_ColorMat1, m_illu1, Cam7D::k_ColorMat2, m_illu2
	);

	// Camera calibration (only one for 7D)
	CamProfile::m_camCalibrationMat = {{
		{ 1.0186, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0.9917 }
	}};

	// HSV maps
	CamProfile::m_profileLook = std::make_shared<HSVMap>(
		36, 8, 16, Cam7D::k_ProfileLookTableData);
	return;
}

// Forward matrix
const Mat3x3 Cam7D::k_ForwardMat1 = {{//@ illu1
	{  0.5445,  0.3536,  0.0662 },
	{  0.1106,  1.0136, -0.1242 },
	{ -0.0374, -0.3559,  1.2184 }
}};
const Mat3x3 Cam7D::k_ForwardMat2 = {{//@ illu2
	{  0.7415,  0.1533,  0.0695 },
	{  0.2499,  0.9997, -0.2497 },
	{ -0.0022, -0.1933,  1.0207 }
}};

// Color matrix
const Mat3x3 Cam7D::k_ColorMat1 = {{//@ illu1
	{  1.1620, -0.6350, 0.0005 },
	{ -0.2558,  1.0146, 0.2813 },
	{  0.0024,  0.0858, 0.6926 }
}};
const Mat3x3 Cam7D::k_ColorMat2 = {{//@ illu2
	{  0.6844, -0.0996, -0.0856 },
	{ -0.3876,  1.1761,  0.2396 },
	{ -0.0593,  0.1772,  0.6198 }
}};

// Camera HSV profile data
#include "profLookMap.hpp"
