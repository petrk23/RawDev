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
#include "Mat3x3.hpp"
#include "Color.hpp"

#include <cassert>

/// <summary>
/// Unit matrix constant
/// </summary>
const Mat3x3 Mat3x3::kUNIT = {{
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, 1 }
}};

Mat3x3 Mat3x3::operator*(const Mat3x3& mat) const
{
	Mat3x3 res;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				res.m_mat[i][j] += m_mat[i][k] * mat.m_mat[k][j];
			}
		}
	}
	return res;
}

Color::RGB64 Mat3x3::operator*(const Color::RGB64& vec) const
{
	Color::RGB64 value{ 0 };

	// Multiply matrix with vector
	value.r = m_mat[0][0] * vec.r + m_mat[0][1] * vec.g + m_mat[0][2] * vec.b;
	value.g = m_mat[1][0] * vec.r + m_mat[1][1] * vec.g + m_mat[1][2] * vec.b;
	value.b = m_mat[2][0] * vec.r + m_mat[2][1] * vec.g + m_mat[2][2] * vec.b;
	return value;
}

Color::RGB64 Mat3x3::operator*(const Color::CIEXYZ& vec) const
{
	Color::RGB64 value{ 0 };

	// Multiply matrix with vector
	value.r = m_mat[0][0] * vec.x + m_mat[0][1] * vec.y + m_mat[0][2] * vec.z;
	value.g = m_mat[1][0] * vec.x + m_mat[1][1] * vec.y + m_mat[1][2] * vec.z;
	value.b = m_mat[2][0] * vec.x + m_mat[2][1] * vec.y + m_mat[2][2] * vec.z;
	return value;
}

/*
Using matrix to convert RGB to XYZ tristimulus
*/
Color::CIEXYZ Mat3x3::toXYZ(const Color::RGB64& vec) const
{
	Color::CIEXYZ value;
	value.x = m_mat[0][0] * vec.r + m_mat[0][1] * vec.g + m_mat[0][2] * vec.b;
	value.y = m_mat[1][0] * vec.r + m_mat[1][1] * vec.g + m_mat[1][2] * vec.b;
	value.z = m_mat[2][0] * vec.r + m_mat[2][1] * vec.g + m_mat[2][2] * vec.b;
	return value;
}

void Mat3x3::operator/=(const double val)
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			m_mat[i][j] /= val;
		}
	}
	return;
}

double Mat3x3::det() const
{
	// Sarrus rule for 3x3
	return (m_mat[0][0] * m_mat[1][1] * m_mat[2][2])
		+ (m_mat[0][1] * m_mat[1][2] * m_mat[2][0])
		+ (m_mat[0][2] * m_mat[1][0] * m_mat[2][1])
		- (m_mat[2][0] * m_mat[1][1] * m_mat[0][2])
		- (m_mat[2][1] * m_mat[1][2] * m_mat[0][0])
		- (m_mat[2][2] * m_mat[1][0] * m_mat[0][1]);
}

Mat3x3 Mat3x3::inverse(void) const
{
	Mat3x3 res;

	if (isDiagonal() == true) {
		for (int i = 0; i < 3; ++i) {
			res.m_mat[i][i] = 1 / m_mat[i][i];
		}
	}
	else if(isInversible() == true) {
		assert(isDiagonal() == false);
		assert(det() != 0.0); // Is inversible

		// Kramers rule
		res.m_mat[0][0] = m_mat[1][1] * m_mat[2][2] - m_mat[2][1] * m_mat[1][2];
		res.m_mat[0][1] = m_mat[0][2] * m_mat[1][2] - m_mat[2][2] * m_mat[0][1];
		res.m_mat[0][2] = m_mat[0][1] * m_mat[1][2] - m_mat[1][1] * m_mat[0][2];
		res.m_mat[1][0] = m_mat[1][2] * m_mat[2][1] - m_mat[2][2] * m_mat[1][0];
		res.m_mat[1][1] = m_mat[0][0] * m_mat[2][2] - m_mat[2][0] * m_mat[0][2];
		res.m_mat[1][2] = m_mat[0][2] * m_mat[1][0] - m_mat[1][2] * m_mat[0][0];
		res.m_mat[2][0] = m_mat[1][0] * m_mat[2][1] - m_mat[2][0] * m_mat[1][1];
		res.m_mat[2][1] = m_mat[0][1] * m_mat[2][0] - m_mat[2][1] * m_mat[0][0];
		res.m_mat[2][2] = m_mat[0][0] * m_mat[1][1] - m_mat[1][0] * m_mat[0][1];
		res /= det();
	}
	return res;
}

bool Mat3x3::isDiagonal() const
{
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (i == j && m_mat[i][j] == 0)
				return false;
			else if (i != j && m_mat[i][j] != 0) {
				return false;
			}
		}
	}
	return true;
}
