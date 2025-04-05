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

#include "NonCopyable.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string_view>

class Logger : public NonCopyable {
    // std::endl type specificaton for operator<<.
    using EndLineType = std::basic_ostream<char>&(std::basic_ostream<char>&);

    static constexpr int k_indentSpaces = 4;
    static constexpr std::string_view k_bulletMark{"* "};
    static_assert(k_bulletMark.length() < k_indentSpaces,
        "Bullet mark is longer than space for indent.");
    enum class FirstLine { None, New, Continued };

    std::ostream* m_osptr;
    FirstLine m_firstLine = FirstLine::New;
    int m_indentLevel = 0;
    bool m_enabled = false;

public:
    Logger(std::ostream& os);

    [[nodiscard]] bool isEnabled() const;
    [[nodiscard]] bool isItemizeMode() const;
    [[nodiscard]] bool isFirstLine() const;
    [[nodiscard]] auto firstLineType() const;

    void setEnabled(bool enabled);
    void indent();
    void unindent();
    void newline();

    template<typename T>
    Logger& operator<<(const T& val);
    Logger& operator<<(EndLineType&);

private:
    void prepareFirstLine();
    void fillWithIndentSpaces(int spaceCount) const;
};

////////////////////////////////////////////////////////////////////////////////

inline Logger::Logger(std::ostream& os)
    : m_osptr{std::addressof(os)}
{
}

/// <summary>
/// Output operator for any content.
/// </summary>
/// <typeparam name="T">Type of the logged value</typeparam>
/// <param name="val">Value to be logged</param>
/// <returns>Logger for futher usage</returns>
template<typename T>
Logger& Logger::operator<<(const T& val)
{
    if (isEnabled()) {
        if (isFirstLine()) {
            prepareFirstLine();
        }
        *m_osptr << val;
    }
    return *this;
}

inline void Logger::indent()
{
    m_indentLevel++;
}

inline void Logger::unindent()
{
    m_indentLevel--;
}

inline bool Logger::isEnabled() const
{
    return m_enabled;
}

inline void Logger::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

inline bool Logger::isItemizeMode() const
{
    return m_indentLevel > 0;
}

inline bool Logger::isFirstLine() const
{
    return m_firstLine != FirstLine::None;
}

inline auto Logger::firstLineType() const
{
    return m_firstLine;
}

inline void Logger::fillWithIndentSpaces(int spaceCount) const
{
    std::fill_n(std::ostream_iterator<char>(*m_osptr), spaceCount, ' ');
}
