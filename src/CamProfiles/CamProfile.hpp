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
#pragma once

#include <string>
#include <memory>

#include "Color.hpp"
#include "Structures/Rect.hpp"
#include "Structures/Mat3x3.hpp"
#include "Structures/HSVMap.hpp"
#include "CFAPattern.hpp"
#include "CamID.hpp"

class CamProfile
{
protected:
	// CFA pattern
	CFAPattern m_cfa;

	// Channel ranges
	Color::RGB64 m_black, m_white;
	double m_baseExposure;

	// Sensor active area
	Rect m_activeArea;

	// Final image crop
	Rect m_crop;

	// Calibration illuminators
	double m_illu1, m_illu2;

	// Transform matrices
	Mat3x3 m_forwardMat, m_colorMat, m_camCalibrationMat, m_analogBalance;

	// HSV Maps
	std::shared_ptr<HSVMap> m_hsvMap, m_profileLook;

public:
	CamProfile(); // Constructor as default initializer
	virtual ~CamProfile();
	virtual std::string getCameraModel() const = 0;
	virtual CamID getCameraID() const = 0;

public:
	CFAPattern getCFAPattern() const;
	Color::RGB64 getBlackLevel() const;
	Color::RGB64 getWhiteLevel() const;
	double getBaseExposure() const;
	Rect getActiveArea() const;
	Rect getCrop() const;
	double getIllu1() const;
	double getIllu2() const;
	Mat3x3 getForwardMatrix() const;
	Mat3x3 getColorMatrix() const;
	Mat3x3 getCamCalibrationMatrix() const;
	Mat3x3 getAnalogBalanceMatrix() const;

public: // HSV Maps
	void applyHSVMap(Color::HSV64 &val) const;
	void applyProfileLook(Color::HSV64 &val) const;
	bool hasHSVMaps() const;

public: // Factory
	static std::shared_ptr<CamProfile>
		MakeCamProfile(const std::string &model, double temp);
};

////////////////////////////////////////////////////////////////////////////////

inline CFAPattern CamProfile::getCFAPattern() const
{
	return m_cfa;
}

inline Color::RGB64 CamProfile::getBlackLevel() const
{
	return m_black;
}

inline Color::RGB64 CamProfile::getWhiteLevel() const
{
	return m_white;
}

inline double CamProfile::getBaseExposure() const
{
	return m_baseExposure;
}

inline Rect CamProfile::getActiveArea() const
{
	return m_activeArea;
}

inline Rect CamProfile::getCrop() const
{
	return m_crop;
}

inline double CamProfile::getIllu1() const
{
	return m_illu1;
}

inline double CamProfile::getIllu2() const
{
	return m_illu2;
}

inline Mat3x3 CamProfile::getForwardMatrix() const
{
	return m_forwardMat;
}

inline Mat3x3 CamProfile::getColorMatrix() const
{
	return m_colorMat;
}

inline Mat3x3 CamProfile::getCamCalibrationMatrix() const
{
	return m_camCalibrationMat;
}

inline Mat3x3 CamProfile::getAnalogBalanceMatrix() const
{
	return m_analogBalance;
}
