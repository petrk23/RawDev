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

struct CFAPattern
{
	enum class Filter;
	enum class Color;

	CFAPattern(Filter filter);
	Color at(int row, int col) const;
	Filter getFilter() const;

private:
	Filter m_filter;
	Color m_cfa[2][2];
};

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// CFA structure
/// </summary>
/// <remarks>
/// All these filter configurations are equivalent.
/// They only differs in moving rows and/or columns by one.
/// The patterns remains the same. So nothing changes
/// for interpolation. This only affects what color filter
/// is on specific positions.
/// </remarks>
enum class CFAPattern::Filter
{
	RGGB, GBRG, BGGR, GRBG
};

/// <summary>
/// Color of the filters in the CFA
/// </summary>
/// <remarks>
/// Ordered intentionally in this way, as we want to process
/// greens together in the switch default section for the most
/// time of processing.
///
/// The suffix _R and _B annotates on which line the green is.
/// This is very useful to know for demosaicing algorithms.
/// </remarks>
enum class CFAPattern::Color
{
	RED, BLUE, GREEN_R, GREEN_B
};

/// <summary>
/// Get the filter color on a specific position
/// </summary>
/// <param name="row">Row</param>
/// <param name="col">Column</param>
/// <returns>Filter color on row and column</returns>
inline CFAPattern::Color CFAPattern::at(int row, int col) const
{
	return m_cfa[row & 1][col & 1];
}

/// <summary>
/// Get raw filter array config
/// </summary>
/// <returns>Filter array configuration</returns>
inline CFAPattern::Filter CFAPattern::getFilter() const
{
	return m_filter;
}
