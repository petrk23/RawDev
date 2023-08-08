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
#include "Path.hpp"
using std::string;
using std::ostream;

#ifdef _WIN32
	/// <summary>
	/// Path separator for windows
	/// </summary>
	const char Path::k_DirSeparator = '\\';
#else
	/// <summary>
	/// Linux, Unix, Mac path separator
	/// </summary>
	const char Path::k_DirSeparator = '/';
#endif

/// <summary>
/// Process path and find filename and extension
/// </summary>
void Path::processPath()
{
	string::reverse_iterator it = m_Path.rbegin(), endit = m_Path.rend();
	m_Extension = std::string(); // Reset to initial
	bool firstDot = false;

	while(it != endit && *it != m_DirSeparator)
	{
		if (firstDot == false && *it == '.')
		{
			m_Extension = m_Path.substr(endit - it);
			firstDot = true; // First dot found
		}
		++it; // Goto next char backwards
	}
	m_FileName = m_Path.substr(endit - it);
	return;
}

/// <summary>
/// Changes the file extension
/// </summary>
/// <param name="extension">Extension with or without dot</param>
void Path::setExtension(const std::string &extension)
{
	const size_t extLen = m_Extension.length();
	if (extLen > 0)
		m_Path = m_Path.substr(0, m_Path.length() - extLen - 1);
	if (!extension.empty() && extension[0] != '.')
		m_Path.append(".");
	m_Path.append(extension);
	processPath(); // Reprocess new path
}
