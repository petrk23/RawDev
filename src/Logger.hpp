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

#include <iostream>
#include <fstream>

class Logger
{
public:
	// std::endl type specification for operator<<.
	typedef std::basic_ostream<char>& (EndLineType)(std::basic_ostream<char>&);

public: // Constants
	static constexpr int kINDENT_SPACES = 4;
	static const char kBULLET_STYLE[];

public:
	enum FirstLine { NONE, NEW, CONTINUED };

private:
	std::ostream& m_os;
	int m_indentLevel;
	bool m_enabled;
	FirstLine m_firstLine;

public:
	Logger(std::ostream& os);

	// Public logger status
	bool isEnabled() const;
	void setEnabled(bool enabled);
	bool isItemizeMode() const;
	bool isFirstLine() const;
	FirstLine firstLineType() const;

	// Operations
	void indent();
	void unindent();
	void newline();

	// Output operators
	template<typename T>
	Logger& operator<<(const T& val);
	Logger& operator<<(EndLineType&);

private:
	void prepareFirstLine();
};

////////////////////////////////////////////////////////////////////////////////

inline Logger::Logger(std::ostream& os)
	: m_os(os), m_indentLevel(0)
	, m_enabled(false), m_firstLine(FirstLine::NEW)
{ }

/// <summary>
/// Output operator for any content.
/// </summary>
/// <typeparam name="T">Type of the logged value</typeparam>
/// <param name="val">Value to be logged</param>
/// <returns>Logger for further usage</returns>
template<typename T>
Logger& Logger::operator<<(const T& val)
{
	if (isEnabled())
	{
		if (isFirstLine())
			prepareFirstLine();
		m_os << val;
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
	return m_firstLine != FirstLine::NONE;
}

inline Logger::FirstLine Logger::firstLineType() const
{
	return m_firstLine;
}
