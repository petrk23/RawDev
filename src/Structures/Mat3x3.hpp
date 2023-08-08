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

namespace Color { struct RGB64; struct CIEXYZ; };

class Mat3x3 {
public:
	static const Mat3x3 kUNIT;
	double m_mat[3][3] = {}; // Zeroed

public:
	Mat3x3 operator* (const Mat3x3&) const;
	Color::RGB64 operator*(const Color::RGB64&) const;
	Color::RGB64 operator*(const Color::CIEXYZ&) const;
	Color::CIEXYZ toXYZ(const Color::RGB64&) const;
	void operator /= (const double val);
	double det() const;
	Mat3x3 inverse() const;
	bool isInversible() const;
	bool isDiagonal() const;
};

///////////////////////////////////////////////////////////////////////////////

inline bool Mat3x3::isInversible() const {
	return det() != 0.0;
}
