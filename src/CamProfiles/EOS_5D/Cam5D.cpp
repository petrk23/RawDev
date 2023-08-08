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
#include "Cam5D.hpp"
#include "Utils.hpp"

/// <summary>
/// Return camera model identifier
/// </summary>
/// <returns>Model identifier string</returns>
std::string Cam5D::getCameraModel() const
{
	return "Canon EOS 5D";
}

/// <summary>
/// Return camera unique ID
/// </summary>
/// <returns>Unique camera id for this app</returns>
CamID Cam5D::getCameraID() const
{
	return CamID::EOS_5D;
}

/// <summary>
/// Load EOS  calibration info
/// </summary>
Cam5D::Cam5D(double temperature)
{
	// Levels
	CamProfile::m_black = { 128, 128, 128 };
	CamProfile::m_white = { 3692, 3692, 3692 };
	CamProfile::m_baseExposure = 0.0; // ignoring the 0.25 to match neutral grey

	// Active area on the sensor (from exif)
	CamProfile::m_activeArea = { 90, 34, 4476, 2954 };

	// Camera crop
	// CamProfile::m_crop = Rect::CreateRect(100, 39, 4368, 2912);
	// but for clean output it must be tighter...
	CamProfile::m_crop = Rect::CreateRect(100, 40, 4368, 2908);

	// Matrix illuminants
	CamProfile::m_illu1 = 2856.0; // A
	CamProfile::m_illu2 = 6504.0; // D65

	// Forward matrix
	CamProfile::m_forwardMat = Utils::interColorMat(temperature,
		Cam5D::k_ForwardMat1, m_illu1, Cam5D::k_ForwardMat2, m_illu2);

	// Color matrix
	CamProfile::m_colorMat = Utils::interColorMat(temperature,
		Cam5D::k_ColorMat1, m_illu1, Cam5D::k_ColorMat2, m_illu2);

	// Camera calibration (only one for 5D)
	CamProfile::m_camCalibrationMat = {{
		{ 0.9804, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1.0318 }
	}};
	return;
}

// Forward matrices
const Mat3x3 Cam5D::k_ForwardMat1 = {{//@ illu1
	{ 0.8141,  0.0349,  0.1153 },
	{ 0.3909,  0.6662, -0.0571 },
	{ 0.1095, -0.2533,  0.9689 }
}};
const Mat3x3 Cam5D::k_ForwardMat2 = {{//@ illu2
	{ 0.7903,  0.0519,  0.1221 },
	{ 0.3935,  0.7150, -0.1085 },
	{ 0.0650, -0.1350,  0.8952 }
}};

// Color matrices
const Mat3x3 Cam5D::k_ColorMat1 = {{//@ illu1
	{  0.7284, -0.1569, -0.0425 },
	{ -0.6726,  1.4015,  0.2993 },
	{ -0.0926,  0.1258,  0.7774 }
}};
const Mat3x3 Cam5D::k_ColorMat2 = {{//@ illu2
	{  0.6347, -0.0479, -0.0972 },
	{ -0.8297,  1.5954,  0.2480 },
	{ -0.1968,  0.2131,  0.7649 }
}};
