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
#include "Color.hpp"
#include "Utils.hpp"
#include "Structures/Mat3x3.hpp"

#include <cmath>

// D50 tristimulus
const Color::CIEXYZ Color::D50 = { 0.9642, 1.0000, 0.8251 };

const Mat3x3 Color::XYZtoSRGB = {{// Conversion XYZ@D50 to sRGB Bradford adapted
	{  3.1338561, -1.6168667, -0.4906146 },
	{ -0.9787684,  1.9161415,  0.0334540 },
	{  0.0719453, -0.2289914,  1.4052427 }
}};

const Mat3x3 Color::XYZtoARGB = {{// Conversion XYZ@D50 to aRGB Bradford adapted
	{  1.9624274, -0.6105343, -0.3413404 },
	{ -0.9787684,  1.9161415,  0.0334540 },
	{  0.0286869, -0.1406752,  1.3487655 }
}};

const Mat3x3 Color::XYZtoProPhotoRGB = {{// Conversion XYZ@D50 to ProPhotoRGB
	{  1.3459433, -0.2556075, -0.0511118 },
	{ -0.5445989,  1.5081673,  0.0205351 },
	{  0.0000000,  0.0000000,  1.2118128 }
}};

// Conversion XYZ to Lab
static double fn(double val, double white)
{
	double res;

	val /= white; // Relative to white point
	if (val > 0.008856)
		res = pow(val, 1.0/3.0);
	else
		res = (903.3 * val + 16) / 116;
	return res;
}

void Color::XYZ2Lab(
	const CIEXYZ& in, CIELab& out, const CIEXYZ& white)
{
	const double fy = fn(in.y, white.y);
	out.L = fma(116, fy, -16);
	out.a = 500 * (fn(in.x, white.x) - fy);
	out.b = 200 * (fy - fn(in.z, white.z));
	return;
}

Color::CIEXYZ Color::cct2XYZ(const double cct)
{
	// CCT to Luv
	const double u = (0.860117757 + 1.54118254e-4 * cct + 1.28641212e-7 * cct * cct)
		/ (1 + 8.42420235e-4 * cct + 7.08145163e-7 * cct * cct);
	const double v = (0.317398726 + 4.22806245e-5 * cct + 4.20481691e-8 * cct * cct)
		/ (1 - 2.89741816e-5 * cct + 1.61456053e-7 * cct * cct);

	// Luv to xy
	const double x = 3 * u / (2 * u - 8 * v + 4);
	const double y = 2 * v / (2 * u - 8 * v + 4);

	// xy to XYZ
	Color::CIEXYZ xyz;
	xyz.x = x / y; // X
	xyz.y = 1.0;   // Y
	xyz.z = (1 - x - y) / y;
	return xyz;
}

Color::HSV64 Color::rgb2hsv(const Color::RGB64 rgb)
{
	Color::HSV64 res{ 0.0 };

	// Calculate value
	res.val = Utils::max3(rgb.r, rgb.g, rgb.b);
	const double delta = res.val - Utils::min3(rgb.r, rgb.g, rgb.b);

	if (delta < 0.00001) {
		res.sat = 0.0; // Black
	}
	else {
		// Saturation
		res.sat = delta / (res.val == 0.0 ? 1.0 : res.val);

		// Hue
		if (rgb.r == res.val) {
			res.hue = (rgb.g - rgb.b) / delta;
		}
		else if (rgb.g == res.val) {
			res.hue = 2.0 + (rgb.b - rgb.r) / delta;
		}
		else if (rgb.b == res.val) {
			res.hue = 4.0 + (rgb.r - rgb.g) / delta;
		}
		res.hue /= 6.0;

		// If hue is negative, then warp
		if (res.hue < 0.0) {
			res.hue += 1.0;
		}
	}
	return res;
}

Color::RGB64 Color::hsv2rgb(const Color::HSV64 hsv)
{
	const double sector = 6 * hsv.hue; // Sector 0 to 6
	const int sectorIndex = static_cast<int>(sector);
	const double f = sector - sectorIndex; // Fractional part inside sector

	// RGB ramp functions
	const double n = hsv.val * (1 - hsv.sat),
		o = hsv.val * (1 - hsv.sat * f), e = hsv.val * (1 - hsv.sat * (1 - f));

	Color::RGB64 res;
	switch (sectorIndex) {
	case 1:
		res = { o, hsv.val, n };
		break;
	case 2:
		res = { n,  hsv.val, e };
		break;
	case 3:
		res = { n, o, hsv.val };
		break;
	case 4:
		res = { e, n, hsv.val };
		break;
	case 5:
		res = { hsv.val, n, o };
		break;
	default: // Sector index 0 || 6
		res = { hsv.val, e, n };
		break;
	}
	return res;
}
