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
#include "CFAPattern.hpp"

/// <summary>
/// Initialize CFA structure
/// </summary>
/// <param name="filter">Filter configuration</param>
CFAPattern::CFAPattern(Filter filter)
{
	m_filter = filter;

	switch (filter)
	{
	case Filter::RGGB:
		m_cfa[0][0] = Color::RED;
		m_cfa[0][1] = Color::GREEN_R;
		m_cfa[1][0] = Color::GREEN_B;
		m_cfa[1][1] = Color::BLUE;
		break;
	case Filter::GBRG:
		m_cfa[0][0] = Color::GREEN_B;
		m_cfa[0][1] = Color::BLUE;
		m_cfa[1][0] = Color::RED;
		m_cfa[1][1] = Color::GREEN_R;
		break;
	case Filter::BGGR:
		m_cfa[0][0] = Color::BLUE;
		m_cfa[0][1] = Color::GREEN_B;
		m_cfa[1][0] = Color::GREEN_R;
		m_cfa[1][1] = Color::RED;
		break;
	default: // Color::GRBG
		m_cfa[0][0] = Color::GREEN_R;
		m_cfa[0][1] = Color::RED;
		m_cfa[1][0] = Color::BLUE;
		m_cfa[1][1] = Color::GREEN_B;
		break;
	}
	return;
}
