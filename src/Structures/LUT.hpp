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

#include <cmath>

template<typename T>
class LUT;

using LUTd = LUT<double>;

template<typename T>
class LUT
{
	int sampleMax;
	T* m_lut;

public:
	 LUT(int samples);
	~LUT(void);

public:
	T& operator[] (const int sample);
	T operator[] (const double input);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline LUT<T>::LUT(int samples)
	: sampleMax(-1), m_lut(nullptr)
{
	if (samples > 0) {
		m_lut = new T[samples];
		sampleMax = samples - 1;
	}
	return;
}

template<typename T>
inline LUT<T>::~LUT(void)
{
	if (m_lut != nullptr) {
		delete[] m_lut;
		m_lut = nullptr;
	}
	return;
}

template<typename T>
inline T& LUT<T>::operator[] (const int sample)
{
	if (sample <= 0)
		return m_lut[0];
	else if (sample >= sampleMax)
		return m_lut[sampleMax];
	return m_lut[sample];
}

template<typename T>
T LUT<T>::operator[] (const double input)
{
	const int index = static_cast<int>(input * sampleMax);
	if (index <= 0)
		return m_lut[0];
	else if (index >= sampleMax)
		return m_lut[sampleMax];

	// Linear interpolate
	const double diff = fma(input, sampleMax, -index);
	const T val1 = m_lut[index];
	const T val2 = m_lut[index + 1] - val1;
	return val1 + val2 * diff;
}
