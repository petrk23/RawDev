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

#include "NonCopyable.hpp"
#include "Color.hpp"
#include "Utils.hpp"

#include <cassert>

template<typename T>
class Array2D : NonCopyable
{
	int m_width, m_height;
	T** m_rowDirPtr, * m_colBasePtr;

	// Construct & Destruct
public:
	Array2D(void);
	Array2D(int, int, bool zero = false);
	Array2D(const Array2D&);
	~Array2D(void);

public: // Public interface
	void operator() (int, int, bool zero = false);
	T* operator[] (int row);
	const T* operator[] (int row) const;
	int getWidth(void) const;
	int getHeight(void) const;
	bool inside(int row, int col) const;


private: // Private interface
	void init(int, int);
	void initEmpty(void);
	void alloc(void);
	void destroy(void);
	void fillRowDirectory(void);
};

////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline Array2D<T>::Array2D(void) {
	initEmpty();
	return;
}

template<typename T>
inline Array2D<T>::Array2D(int width, int height, bool zero) {
	initEmpty(); // Needed to avoid releasing nonvalid pointers!
	operator()(width, height, zero);
	return;
}

template<typename T>
Array2D<T>::Array2D(const Array2D& src)
{
	if (src.m_width > 0 || src.m_height > 0) {
		init(src.m_width, src.m_height);
		const size_t size = sizeof(T) * m_width * m_height;
		Utils::memoryCopy(m_colBasePtr, src.m_colBasePtr, size);
	}
	else initEmpty();
	return;
}

template<typename T>
inline Array2D<T>::~Array2D(void) {
	destroy();
}

template<typename T>
void Array2D<T>::operator() (int width, int height, bool zero)
{
	destroy(); // Destroy old if present
	if (width > 0 && height > 0) {
		init(width, height);
		if (zero == true) {
			memset(m_colBasePtr, 0, sizeof(T) * width * height);
		}
	}
	else initEmpty();
	return;
}

template<typename T>
inline T* Array2D<T>::operator[] (int row) {
	assert(row >= 0 && row < m_height);
	return m_rowDirPtr[row];
}

template<typename T>
inline const T* Array2D<T>::operator[] (int row) const {
	assert(row >= 0 && row < m_height);
	return m_rowDirPtr[row];
}

template<typename T>
inline int Array2D<T>::getWidth(void) const {
	return m_width;
}

template<typename T>
inline int Array2D<T>::getHeight(void) const {
	return m_height;
}

template<typename T>
inline bool Array2D<T>::inside(int row, int col) const {
	return row >= 0 && row < m_height
		&& col >= 0 && col < m_width;
}

template<typename T>
void Array2D<T>::init(int width, int height)
{
	assert(width > 0 && height > 0);
	m_width = width;
	m_height = height;
	alloc(); fillRowDirectory();
	return;
}

template<typename T>
void Array2D<T>::initEmpty(void)
{
	m_width = 0; m_height = 0;
	m_rowDirPtr = nullptr;
	m_colBasePtr = nullptr;
	return;
}

template<typename T>
void Array2D<T>::alloc(void)
{
	const int datasize = m_height * m_width;
	m_rowDirPtr = new T * [m_height];
	m_colBasePtr = new T[datasize];
	return;
}

template<typename T>
void Array2D<T>::destroy(void)
{
	if (m_rowDirPtr != nullptr) {
		delete[] m_rowDirPtr;
		m_rowDirPtr = nullptr;
	}
	if (m_colBasePtr != nullptr) {
		delete[] m_colBasePtr;
		m_colBasePtr = nullptr;
	}
	return;
}

template<typename T>
void Array2D<T>::fillRowDirectory(void)
{
	const int height = m_height;
	T* rowPointer = m_colBasePtr;

	for (int d = 0; d < height; d++) {
		m_rowDirPtr[d] = rowPointer;
		rowPointer += m_width;
	}
	return;
}
