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

#include <algorithm>

#include <cmath>
#include <ctime>
#include <cstring>
#include <cstdint>

class Mat3x3;

namespace Utils {
	inline bool even(int number) {
		return (number & 1) == 0;
	}

	inline bool odd(int number) {
		return !even(number);
	}

	inline double incAverage(
		const double ave, const double value, const int n)
	{
		return ave + (value - ave) / n;
	}

	template<typename T>
	inline T sqr(T value) {
		return value * value;
	}

	inline double val2EV(const double val) {
		return log2(val);
	}

	inline double EV2Val(const double ev) {
		return pow(2, ev);
	}

	template<typename T>
	inline T min3(T val1, T val2, T val3) {
		return std::min(std::min(val1, val2), val3);
	}

	template<typename T>
	inline T max3(T val1, T val2, T val3) {
		return std::max(std::max(val1, val2), val3);
	}

	template<typename T> // XOR median algorithm
	inline double median(T a, T b, T c) {
		if ((a > b) != (a > c))
			return a;
		else if ((b > a) != (b > c))
			return b;
		return c;
	}

	template<typename T>
	inline T clip(T val, T min, T max) {
		return std::max(min, std::min(val, max));
	}

	int compareDouble(const void*, const void*);

	inline double linearInter(double x,
		double x0, double y0, double x1, double y1)
	{
		return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
	}

	Mat3x3 interColorMat(double, const Mat3x3&, double, const Mat3x3&, double);
	Mat3x3 interpolateMatrices(double, const Mat3x3&, double, const Mat3x3&, double);

	inline void memoryCopy(void* dest, const void* src, size_t bytes)
	{
	#ifdef _WIN32
		memcpy_s(dest, bytes, src, bytes);
	#else
		std::memcpy(dest, src, bytes);
	#endif
	}

	inline uint16_t byteSwapU16(uint16_t val)
	{
	#ifdef _WIN32
		return _byteswap_ushort(val);
	#else // We are on Unix. Also GCC and CLANG buildin.
		return __builtin_bswap16(val);
	#endif
	}

	inline struct tm* localTime(const time_t* time, struct tm* buf)
	{
		struct tm* res;

	#ifdef _WIN32
		localtime_s(buf, time);
		res = buf;
	#else
		res = localtime_r(time, buf);
	#endif
		return res;
	}
};
