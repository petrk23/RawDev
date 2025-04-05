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

#include "Path.hpp"

#ifdef _WIN32
/// <summary>
/// Path separator for windows
/// </summary>
const char Path::k_dirSeparator = '\\';
#else
/// <summary>
/// Linux, Unix, Mac path separator
/// </summary>
const char Path::k_dirSeparator = '/';
#endif

/// <summary>
/// Process path and find filename and extension
/// </summary>
void Path::processPath()
{
    auto it = m_path.crbegin();
    auto endit = m_path.crend();
    bool firstDot = false;

    m_extOffset = m_path.length();
    for (; it != endit && *it != m_dirSeparator; ++it) {
        if (!firstDot && *it == '.') {
            m_extOffset = endit - it;
            firstDot = true; // First dot found
        }
    }
    m_fileOffset = endit - it;
}

/// <summary>
/// Changes the file extension
/// </summary>
/// <param name="extension">Extension with or without dot</param>
void Path::setExtension(std::string_view newExtension)
{
    if (!newExtension.empty() && newExtension[0] == '.') {
        newExtension.remove_prefix(1); // Remove first dot from imput
    }

    if (hasExtension()) {
        if (!newExtension.empty()) {
            m_path.replace(m_extOffset, std::string::npos, newExtension);
            recalcExtensionOffset();
        }
        else {
            m_path.erase(m_extOffset - 1); // Erase ext with dot
            m_extOffset = m_path.length();
        }
    }
    else if (!newExtension.empty()) {
        m_path.push_back('.');
        m_path.append(newExtension);
        recalcExtensionOffset();
    }
}
