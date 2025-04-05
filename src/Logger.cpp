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

#include "Logger.hpp"

#include <cassert>
#include <iterator>

/// <summary>
/// Output operator font endline character.
/// </summary>
/// <param name="el">Endline function as parameter</param>
/// <returns>Logger for futher usage</returns>
Logger& Logger::operator<<(EndLineType& el)
{
    if (isEnabled()) {
        *m_osptr << el;
        m_firstLine = FirstLine::New;
    }
    return *this;
}

/// <summary>
/// Prepare for output on new line
/// </summary>
void Logger::prepareFirstLine()
{
    assert(isEnabled());

    const int spaceCount = (m_indentLevel - 1) * k_indentSpaces;
    fillWithIndentSpaces(spaceCount);

    if (isItemizeMode()) {
        switch (m_firstLine) {
        case FirstLine::New:
            *m_osptr << k_bulletMark;
            break;
        case FirstLine::Continued:
            fillWithIndentSpaces(// Continued new line without bullet
                static_cast<int>(k_bulletMark.length()));
            break;
        default:
            break;
        }
    }
    m_firstLine = FirstLine::None;
}

/// <summary>
/// Newline for text
/// </summary>
void Logger::newline()
{
    if (isEnabled()) {
        *m_osptr << std::endl; // NOLINT(performance-avoid-endl)
        m_firstLine = FirstLine::Continued;
    }
}
