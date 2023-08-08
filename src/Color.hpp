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

#include <cinttypes>
#include <cstdlib>

class Mat3x3;

namespace Color {
	struct CIEXYZ { double x, y, z; };

	struct CIELab
	{
		double L, a, b;

		// Distance functions
		double dl(const CIELab& val) const {
			return std::abs(L - val.L);
		}
		double dc(const CIELab& val) const {
			const double adiff = a - val.a, bdiff = b - val.b;
			return adiff * adiff + bdiff * bdiff;
		}
	};

	struct HSV64 { double hue, sat, val; };
	struct RGB64 { double r, g, b; };
	struct RGB16 { uint16_t r, g, b; };
	struct RGB8  { uint8_t  r, g, b; };

	////////////////////////////////////////////////////////////////////////////
	// Constants
	////////////////////////////////////////////////////////////////////////////

	extern const CIEXYZ D50;

	/* Colorspace conversion matrices */
	extern const Mat3x3 XYZtoSRGB;
	extern const Mat3x3 XYZtoARGB;
	extern const Mat3x3 XYZtoProPhotoRGB;

	////////////////////////////////////////////////////////////////////////////
	// Functions
	////////////////////////////////////////////////////////////////////////////

	void XYZ2Lab(const CIEXYZ&, CIELab&, const CIEXYZ&);
	Color::CIEXYZ cct2XYZ(const double cct);
	Color::HSV64 rgb2hsv(const Color::RGB64 rgbValue);
	Color::RGB64 hsv2rgb(const Color::HSV64 hsv);

	inline double kelvin2mired(const double temp) {
		return 1e6 / temp;
	}
};
