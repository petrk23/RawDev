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

#include <memory>

#include "NonCopyable.hpp"
#include "Color.hpp"

class HSVMap : NonCopyable
{
public: // Scale HSV in double precision
	struct HSV64Scale {
		double hueShift, satScale, valScale;
	};

public:
	HSVMap(int hueDim, int satDim, int valDim, const HSV64Scale *data);
	HSVMap(int hueDim, int satDim, int valDim,
		const HSV64Scale *data1, double illu1,
		const HSV64Scale *data2, double illu2,
		double temperature);
	void transform(Color::HSV64 &hsv) const;

private:
	int m_hueDim, m_satDim, m_valDim;
	std::unique_ptr<HSV64Scale[]> m_data;

private: // Interpolation helper functions
	HSV64Scale map(int h, int s, int v) const;
	HSV64Scale interSlice(int, int, int, double, double) const;
	static void interScales(HSV64Scale &, const HSV64Scale &, const double);
	static void MakeOrderedIllu(const HSV64Scale *&data1, double &illu1,
		const HSV64Scale *&data2, double &illu2);
	static void Scale(const HSV64Scale &scale, Color::HSV64 &val);
};

////////////////////////////////////////////////////////////////////////////////

inline HSVMap::HSV64Scale HSVMap::map(int h, int s, int v) const
{
	const int offset = m_satDim * (v * m_hueDim + h) + s;
	return m_data[offset];
}
