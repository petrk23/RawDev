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

#include <string>
#include <ostream>

class Path
{
	static const char k_DirSeparator;

	std::string m_Path;
	std::string m_Extension;
	std::string m_FileName;
	char m_DirSeparator;

public:
	Path(char dirSeparator=k_DirSeparator);
	Path(const char *path, char dirSeparator=k_DirSeparator);
	Path(const std::string &path, char dirSeparator = k_DirSeparator);
	operator std::string() const;
	friend std::ostream &operator<< (std::ostream &os, const Path &path);

public:
	std::string getPath() const;
	void setExtension(const std::string &extension);
	std::string getExtension() const;
	std::string getFileName() const;
	void setDirSeparator(char dirSeparator);
	char getDirSeparator() const;

private:
	void processPath();
};

////////////////////////////////////////////////////////////////////////////////

inline Path::Path(char dirSeparator)
	: m_Path(), m_Extension(), m_FileName(), m_DirSeparator(dirSeparator)
{}

inline Path::Path(const char *path, char dirSeparator)
	: m_Path(path)
	, m_Extension(), m_FileName()
	, m_DirSeparator(dirSeparator)
{
	processPath();
	return;
}

inline Path::Path(const std::string &path, char dirSeparator)
	: m_Path(path)
	, m_Extension(), m_FileName()
	, m_DirSeparator(dirSeparator)
{
	processPath();
	return;
}

inline Path::operator std::string() const
{
	return std::string(m_Path);
}

inline std::string Path::getPath() const
{
	return m_Path;
}

inline std::string Path::getExtension() const
{
	return m_Extension;
}

inline std::string Path::getFileName() const
{
	return m_FileName;
}

inline void Path::setDirSeparator(char dirSeparator)
{
	m_DirSeparator = dirSeparator;
	processPath(); // Reprocess the path
	return;
}

inline char Path::getDirSeparator() const
{
	return m_DirSeparator;
}

inline std::ostream &operator<<(std::ostream &os, const Path &path)
{
	os << path.m_Path;
	return os;
}
