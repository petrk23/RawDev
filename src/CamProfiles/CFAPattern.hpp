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

#include <cassert>

struct CFAPattern {
    enum class Filter;
    enum class Color;

    CFAPattern(Filter filter);
    Color operator()(int row, int col) const;
    Filter getFilter() const;

private:
    using Pattern = Color[2][2];
    static const Pattern k_cfaList[];
    const Pattern* m_pattern; // Current CFA pattern pointer
    Filter m_filter;
};

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// CFA structure
/// </summary>
/// <remarks>
/// All theese filter configurations are equivalent.
/// They only differs in moving rows and/or columns by one.
/// The patterns reamin the same. So nothing changes
/// for interpolation. This only affects what color filter
/// is on specific positions.
/// </remarks>
enum class CFAPattern::Filter {
    RGGB,
    GBRG,
    BGGR,
    GRBG
};

/// <summary>
/// Color of the filters in the CFA
/// </summary>
/// <remarks>
/// Ordered intentionaly in this way, as we want to process
/// greens together in the switch default section for the most
/// time of processing.
///
/// The suffix _R and _B anotates on which line the green is.
/// This is very useful to know for demosaicing algorithms.
/// </remarks>
enum class CFAPattern::Color {
    RED,
    BLUE,
    GREEN_R,
    GREEN_B
};

/// <summary>
/// Construct CFA pattern structure
/// </summary>
/// <param name="filter">Actual filter</param>
inline CFAPattern::CFAPattern(Filter filter)
    : m_pattern{k_cfaList + static_cast<int>(filter)},
      m_filter{filter}
{
}

/// <summary>
/// Get the filter color on a specific position
/// </summary>
/// <param name="row">Row</param>
/// <param name="col">Column</param>
/// <returns>Filter color on row and column</returns>
inline CFAPattern::Color CFAPattern::operator()(int row, int col) const
{
    assert(row >= 0 && col >= 0);
    return (*m_pattern)[row & 1][col & 1];
}

/// <summary>
/// Get raw filter array config
/// </summary>
/// <returns>Filter array configuration</returns>
inline CFAPattern::Filter CFAPattern::getFilter() const
{
    return m_filter;
}
