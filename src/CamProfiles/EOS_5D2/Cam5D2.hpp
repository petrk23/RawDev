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

#include "CamProfiles/CamProfile.hpp"

class Cam5D2 : public CamProfile
{
public:
	explicit Cam5D2(double temperature);
	virtual std::string getCameraModel() const;
	virtual CamID getCameraID() const;

private:
	static const Mat3x3 k_ColorMat1, k_ColorMat2;
	static const HSVMap::HSV64Scale k_ProfileLookTableData[];
};
