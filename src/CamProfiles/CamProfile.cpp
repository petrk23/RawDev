/*
 * This file is part of RawDev;
 * see <https://github.com/petrk23/RawDev>.
 *
 * Copyright (C) 2020-2025 Petr Krajník
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "AllCamProfiles.hpp"
#include "Structures/HSVMap.hpp"

using std::make_shared;
using std::shared_ptr;
using std::string_view;

/// <summary>
/// Apply camera HSV maps if present
/// </summary>
/// <param name="val">Value to be processed</param>
void CamProfile::applyHSVMap(Color::HSV64& val) const
{
    if (m_hsvMap != nullptr) {
        m_hsvMap->transform(val);
    }
}

/// <summary>
/// Apply default profile look if present
/// </summary>
/// <param name="val">Value to be processed</param>
void CamProfile::applyProfileLook(Color::HSV64& val) const
{
    if (m_profileLook != nullptr) {
        m_profileLook->transform(val);
    }
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
/// Camera profile factory by model name
/// </summary>
/// <param name="model">Unique model name</param>
/// <param name="temp">Color temperature</param>
/// <returns>Pointer to new camera profile</returns>
shared_ptr<CamProfile>
CamProfile::MakeCamProfile(const string_view model, double temp)
{
    shared_ptr<CamProfile> profilePtr;

    if (model == Cam1DX::k_camName) {
        profilePtr = make_shared<Cam1DX>(temp);
    }
    else if (model == Cam1DX2::k_camName) {
        profilePtr = make_shared<Cam1DX2>(temp);
    }
    else if (model == Cam5D::k_camName) {
        profilePtr = make_shared<Cam5D>(temp);
    }
    else if (model == Cam5D2::k_camName) {
        profilePtr = make_shared<Cam5D2>(temp);
    }
    else if (model == Cam5D3::k_camName) {
        profilePtr = make_shared<Cam5D3>(temp);
    }
    else if (model == Cam5D4::k_camName) {
        profilePtr = make_shared<Cam5D4>(temp);
    }
    else if (model == Cam5Ds::k_camName) {
        profilePtr = make_shared<Cam5Ds>(temp);
    }
    else if (model == Cam5DsR::k_camName) {
        profilePtr = make_shared<Cam5DsR>(temp);
    }
    else if (model == Cam6D::k_camName) {
        profilePtr = make_shared<Cam6D>(temp);
    }
    else if (model == Cam6D2::k_camName) {
        profilePtr = make_shared<Cam6D2>(temp);
    }
    else if (model == Cam7D::k_camName) {
        profilePtr = make_shared<Cam7D>(temp);
    }
    else if (model == Cam7D2::k_camName) {
        profilePtr = make_shared<Cam7D2>(temp);
    }
    else if (model == Cam77D::k_camName) {
        profilePtr = make_shared<Cam77D>(temp);
    }
    else if (model == Cam80D::k_camName) {
        profilePtr = make_shared<Cam80D>(temp);
    }
    else {
        assert(false && "Unknown camera name.");
    }
    return profilePtr;
}

std::shared_ptr<CamProfile>
CamProfile::MakeCamProfile(CamID id, double temp)
{
    shared_ptr<CamProfile> profilePtr;

    switch (id) {
    case CamID::EOS_1DX:
        profilePtr = std::make_shared<Cam1DX>(temp);
        break;
    case CamID::EOS_1DX2:
        profilePtr = std::make_shared<Cam1DX2>(temp);
        break;
    case CamID::EOS_5D:
        profilePtr = std::make_shared<Cam5D>(temp);
        break;
    case CamID::EOS_5D2:
        profilePtr = std::make_shared<Cam5D2>(temp);
        break;
    case CamID::EOS_5D3:
        profilePtr = std::make_shared<Cam5D3>(temp);
        break;
    case CamID::EOS_5D4:
        profilePtr = std::make_shared<Cam5D4>(temp);
        break;
    case CamID::EOS_5Ds:
        profilePtr = std::make_shared<Cam5Ds>(temp);
        break;
    case CamID::EOS_5DsR:
        profilePtr = std::make_shared<Cam5DsR>(temp);
        break;
    case CamID::EOS_6D:
        profilePtr = std::make_shared<Cam6D>(temp);
        break;
    case CamID::EOS_6D2:
        profilePtr = std::make_shared<Cam6D2>(temp);
        break;
    case CamID::EOS_7D:
        profilePtr = std::make_shared<Cam7D>(temp);
        break;
    case CamID::EOS_7D2:
        profilePtr = std::make_shared<Cam7D2>(temp);
        break;
    case CamID::EOS_77D:
        profilePtr = std::make_shared<Cam77D>(temp);
        break;
    case CamID::EOS_80D:
        profilePtr = std::make_shared<Cam80D>(temp);
        break;
    default:
        assert(false && "Unknown camera ID.");
        break;
    }
    return profilePtr;
}
