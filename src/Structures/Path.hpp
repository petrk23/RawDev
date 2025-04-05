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

#include <ostream>
#include <string>
#include <cassert>

class Path {
    static const char k_dirSeparator;

    std::string m_path;
    using size_type = std::string::size_type;
    size_type m_fileOffset, m_extOffset;
    char m_dirSeparator;

public:
    Path(char dirSeparator = k_dirSeparator);
    Path(std::string path, char dirSeparator = k_dirSeparator);

    operator std::string() const;

    [[nodiscard]] std::string getPath() const;
    [[nodiscard]] std::string getFileName() const;
    [[nodiscard]] std::string getExtension() const;
    [[nodiscard]] char getDirSeparator() const noexcept;

    void setExtension(std::string_view newExtension);
    void setDirSeparator(char dirSeparator);

    [[nodiscard]] bool hasExtension() const;

private:
    void processPath();
    void recalcExtensionOffset();
};

////////////////////////////////////////////////////////////////////////////////

inline Path::Path(char dirSeparator)
    : m_fileOffset{0}, m_extOffset{0}, m_dirSeparator{dirSeparator}
{
}

inline Path::Path(std::string path, char dirSeparator)
    : m_path{std::move(path)}, m_dirSeparator{dirSeparator}
{
    processPath();
}

inline Path::operator std::string() const
{
    return m_path;
}

inline std::string Path::getPath() const
{
    return m_path;
}

inline std::string Path::getFileName() const
{
    return m_path.substr(m_fileOffset);
}

inline std::string Path::getExtension() const
{
    return m_path.substr(m_extOffset);
}

inline void Path::setDirSeparator(char dirSeparator)
{
    m_dirSeparator = dirSeparator;
    processPath(); // Reprocess the path
}

inline bool Path::hasExtension() const
{
    return m_extOffset < m_path.length();
}

inline char Path::getDirSeparator() const noexcept
{
    return m_dirSeparator;
}

inline void Path::recalcExtensionOffset()
{
    const auto pos = m_path.rfind('.');
    m_extOffset = pos + 1; // Dot is not part of the extension text

    // This function expects, that there is a dot in the filename.
    assert(pos != std::string::npos);
    assert(m_extOffset > m_fileOffset);
}

inline std::ostream& operator<<(std::ostream& os, const Path& path)
{
    os << path.getPath();
    return os;
}
