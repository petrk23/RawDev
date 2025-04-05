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

#pragma once

#include "EOS_1DX/Cam1DX.hpp"
#include "EOS_1DX2/Cam1DX2.hpp"
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
#include "EOS_77D/Cam77D.hpp"
#include "EOS_80D/Cam80D.hpp"

constexpr std::string_view k_camNames[] = {
    Cam1DX ::k_camName,
    Cam1DX2::k_camName,
    Cam5D  ::k_camName,
    Cam5D2 ::k_camName,
    Cam5D3 ::k_camName,
    Cam5D4 ::k_camName,
    Cam5Ds ::k_camName,
    Cam5DsR::k_camName,
    Cam6D  ::k_camName,
    Cam6D2 ::k_camName,
    Cam77D ::k_camName,
    Cam7D  ::k_camName,
    Cam7D2 ::k_camName
};

constexpr CamID k_camIDs[] = {
    CamID::EOS_1DX,
    CamID::EOS_1DX2,
    CamID::EOS_5D,
    CamID::EOS_5D2,
    CamID::EOS_5D3,
    CamID::EOS_5D4,
    CamID::EOS_5Ds,
    CamID::EOS_5DsR,
    CamID::EOS_6D,
    CamID::EOS_6D2,
    CamID::EOS_77D,
    CamID::EOS_7D,
    CamID::EOS_7D2,
};
