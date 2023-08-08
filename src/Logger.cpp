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
#include "Logger.hpp"

#include <iterator>
#include <cassert>
using namespace std;

/// <summary>
/// Bullet style when in itemize mode.
/// </summary>
const char Logger::kBULLET_STYLE[] = "* ";

/// <summary>
/// Output operator font endline character.
/// </summary>
/// <param name="el">Endline function as parameter</param>
/// <returns>Logger for further usage</returns>
Logger& Logger::operator<<(EndLineType& el)
{
	if (isEnabled())
	{
		m_os << el;
		m_firstLine = FirstLine::NEW;
	}
	return *this;
}

/// <summary>
/// Prepare for output on new line
/// </summary>
void Logger::prepareFirstLine()
{
	assert(isEnabled());

	// Fill with indent spaces
	const int spaceCount = (m_indentLevel - 1) * kINDENT_SPACES;
	fill_n(ostream_iterator<char>(m_os), spaceCount, ' ');

	// Print bullet if in itemize mode
	if (isItemizeMode())
	{
		switch (m_firstLine)
		{
		case FirstLine::NEW:
			m_os << kBULLET_STYLE;
			break;
		case FirstLine::CONTINUED:
			fill_n(ostream_iterator<char>(m_os), // Align with bullet items
				(sizeof(kBULLET_STYLE) / sizeof(kBULLET_STYLE[0])) - 1, ' ');
			break;
		default:
			break;
		}
	}
	m_firstLine = FirstLine::NONE;
}

/// <summary>
/// Newline for text
/// </summary>
void Logger::newline()
{
	if (isEnabled())
	{
		m_os << endl;
		m_firstLine = FirstLine::CONTINUED;
	}
}
