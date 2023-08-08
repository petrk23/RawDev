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

class Rect
{
public:
	int left, top;
	int right, bottom;

	Rect();
	Rect(int left, int top, int right, int bottom);

	// Dimension getters
	int getWidth() const;
	int getHeight() const;

	// Factory method
	static Rect CreateRect(int left, int top, int width, int height);
};

////////////////////////////////////////////////////////////////////////////////

inline Rect::Rect()
	: left(0), top(0), right(0), bottom(0)
{ }

inline Rect::Rect(int left, int top, int right, int bottom)
	: left(left), top(top), right(right), bottom(bottom)
{ }

inline Rect Rect::CreateRect(int left, int top, int width, int height)
{
	return Rect(left, top, left + width, top + height);
}

inline int Rect::getWidth() const
{
	return right - left;
}

inline int Rect::getHeight() const
{
	return bottom - top;
}
