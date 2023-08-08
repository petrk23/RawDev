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
#include "CamProfile.hpp"

#include "Structures/HSVMap.hpp"

#include "EOS_5D/Cam5D.hpp"
#include "EOS_5D2/Cam5D2.hpp"
#include "EOS_5D3/Cam5D3.hpp"
#include "EOS_5D4/Cam5D4.hpp"
#include "EOS_5Ds/Cam5Ds.hpp"
#include "EOS_5DsR/Cam5DsR.hpp"
#include "EOS_6D/Cam6D.hpp"
#include "EOS_6D2/Cam6D2.hpp"
#include "EOS_7D/Cam7D.hpp"
#include "EOS_7D2/Cam7D2.hpp"

using namespace std;

/// <summary>
/// Default construction of empty profile
/// </summary>
CamProfile::CamProfile()
	: m_cfa(CFAPattern::Filter::RGGB)
	, m_black(), m_white(), m_baseExposure(0.0)
	, m_activeArea(), m_crop()
	, m_illu1(0), m_illu2(0)
	, m_forwardMat(), m_colorMat(), m_camCalibrationMat()
	, m_analogBalance(Mat3x3::kUNIT) // Unit matrix default
	, m_hsvMap(nullptr), m_profileLook(nullptr)
{}

/// <summary>
/// Empty virtual destructor
/// </summary>
CamProfile::~CamProfile()
{ }

/// <summary>
/// Apply camera HSV maps if present
/// </summary>
/// <param name="val">Value to be processed</param>
void CamProfile::applyHSVMap(Color::HSV64 &val) const
{
	if (m_hsvMap != nullptr)
		m_hsvMap->transform(val);
	return;
}

/// <summary>
/// Apply default profile look if present
/// </summary>
/// <param name="val">Value to be processed</param>
void CamProfile::applyProfileLook(Color::HSV64 &val) const
{
	if (m_profileLook != nullptr)
		m_profileLook->transform(val);
	return;
}

/// <summary>
/// Check if one or more HSV maps exists
/// </summary>
/// <returns>True if at least one map exists. Otherwise false.</returns>
bool CamProfile::hasHSVMaps() const
{
	return m_hsvMap != nullptr || m_profileLook != nullptr;
}

/// <summary>
/// Camera profile factory
/// </summary>
/// <param name="model">Unique model name</param>
/// <param name="temp">Color temperature</param>
/// <returns>Pointer to new profile</returns>
shared_ptr<CamProfile>
CamProfile::MakeCamProfile(const string &model, double temp)
{
	shared_ptr<CamProfile> profilePtr;

	if (model == "Canon EOS 5D")
		profilePtr = make_shared<Cam5D>(temp);
	else if (model == "Canon EOS 5D Mark II")
		profilePtr = make_shared<Cam5D2>(temp);
	else if (model == "Canon EOS 5D Mark III")
		profilePtr = make_shared<Cam5D3>(temp);
	else if (model == "Canon EOS 5D Mark IV")
		profilePtr = make_shared<Cam5D4>(temp);
	else if (model == "Canon EOS 5DS")
		profilePtr = make_shared<Cam5Ds>(temp);
	else if (model == "Canon EOS 5DS R")
		profilePtr = make_shared<Cam5DsR>(temp);
	else if (model == "Canon EOS 6D")
		profilePtr = make_shared<Cam6D>(temp);
	else if (model == "Canon EOS 6D Mark II")
		profilePtr = make_shared<Cam6D2>(temp);
	else if (model == "Canon EOS 7D")
		profilePtr = make_shared<Cam7D>(temp);
	else if (model == "Canon EOS 7D Mark II")
		profilePtr = make_shared<Cam7D2>(temp);

	return profilePtr;
}
