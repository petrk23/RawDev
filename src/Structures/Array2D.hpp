/*
 * This file is part of RawDev;
 * see <https://github.com/petrk23/RawDev>.
 *
 * Copyright (C) 2020-2025 Petr Krajník
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <type_traits>

template<typename T>
class Array2D {
    static_assert(
        std::is_object_v<T>, "Array2D content must be a object type.");

public:
    Array2D() noexcept;
    Array2D(int width, int height);
    Array2D(int width, int height, const T& init);
    Array2D(const Array2D& src);
    Array2D(Array2D&& src) noexcept;
    Array2D& operator=(const Array2D& src);
    Array2D& operator=(Array2D&& src) noexcept;

    // Getters and setters
    int getWidth() const noexcept;
    int getHeight() const noexcept;
    size_t getItemCount() const noexcept;
    T* operator[](int row);
    const T* operator[](int row) const;
    bool inside(int row, int col) const noexcept;
    bool empty() const noexcept;

private:
    void construct(int width, int height);
    void setDimensions(int width, int height) noexcept;
    void allocate();
    void clone(const Array2D<T>& src);

    std::unique_ptr<T[]> m_colBasePtr;
    int m_width, m_height;
};

////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline Array2D<T>::Array2D() noexcept
    : m_width{0}, m_height{0}
{
}

template<typename T>
inline Array2D<T>::Array2D(int width, int height)
{
    // Destruction or access to nontrivial uninitialized objects
    // causes errors. For this reason only trivial types are allowed,
    // which doesn't suffer from this problem.
    // BUT REMEMBER: the objects are in a undefined state as they
    // memory is not initialized!
    static_assert(std::is_trivial_v<T>,
        "Uninitialized construction is allowed for trivial types only.");

    construct(width, height);
}

template<typename T>
inline Array2D<T>::Array2D(int width, int height, const T& init)
{
    construct(width, height);
    std::uninitialized_fill_n(m_colBasePtr.get(), getItemCount(), init);
}

template<typename T>
inline Array2D<T>::Array2D(const Array2D& src)
{
    clone(src);
}

template<typename T>
inline Array2D<T>::Array2D(Array2D&& src) noexcept
    : m_colBasePtr(std::move(src.m_colBasePtr)),
      m_width{src.m_width},
      m_height{src.m_height}
{
    src.setDimensions(0, 0); // Make source empty
}

template<typename T>
inline Array2D<T>& Array2D<T>::operator=(const Array2D& src)
{
    if (this != &src) {
        m_colBasePtr.reset(); // Release current memory
        clone(src);           // Copy src content
    }
}

template<typename T>
inline Array2D<T>& Array2D<T>::operator=(Array2D&& src) noexcept
{
    if (this != &src) {
        m_colBasePtr.swap(src.m_colBasePtr);
        std::swap(m_width, src.m_width);
        std::swap(m_height, src.m_height);
    }
    return *this;
}

template<typename T>
inline int Array2D<T>::getWidth() const noexcept
{
    return m_width;
}

template<typename T>
inline int Array2D<T>::getHeight() const noexcept
{
    return m_height;
}

template<typename T>
inline size_t Array2D<T>::getItemCount() const noexcept
{
    return static_cast<size_t>(m_width) * m_height;
    // size_t is needed to prevent possible arithmetic overflow.
}

template<typename T>
inline T* Array2D<T>::operator[](int row)
{
    assert(row >= 0 && row < m_height);
    const size_t offset = static_cast<size_t>(row) * m_width;
    return m_colBasePtr.get() + offset;
}

template<typename T>
inline const T* Array2D<T>::operator[](int row) const
{
    return const_cast<Array2D<T>&>(*this)[row];
}

template<typename T>
inline bool Array2D<T>::inside(int row, int col) const noexcept
{
    return row >= 0 && row < m_height && (col >= 0 && col < m_width);
}

template<typename T>
inline bool Array2D<T>::empty() const noexcept
{
    // It is sufficient to test only one dimension,
    // as they are both zero or nonzero...
    return m_width < 1;
}

template<typename T>
inline void Array2D<T>::construct(int width, int height)
{
    if (width > 0 && height > 0) {
        setDimensions(width, height);
        allocate();
        return;
    }
    throw std::out_of_range("zero or negative array dimension");
}

template<typename T>
inline void Array2D<T>::setDimensions(int width, int height) noexcept
{
    m_width  = width;
    m_height = height;
}

template<typename T>
inline void Array2D<T>::allocate()
{
    assert(m_width > 0 && m_height > 0);
    m_colBasePtr = std::make_unique<T[]>(getItemCount());

    // The assigment operator releases previously alocated memory,
    // if any valid is present.
}

template<typename T>
inline void Array2D<T>::clone(const Array2D<T>& src)
{
    setDimensions(src.m_width, src.m_height);
    assert((m_width > 0 && m_height > 0) || (m_width == 0 && m_height == 0));

    if (!src.empty()) {
        allocate();
        std::uninitialized_copy_n(src.m_colBasePtr.get(),
            src.getItemCount(), m_colBasePtr.get());
    }
}
