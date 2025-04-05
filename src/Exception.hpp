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

#include <string>
#include <sstream>

class Exception
{
    std::string m_module;
    std::string m_file;
    std::string m_message;

public:
    Exception(const std::string& module,
        const std::string& file, const std::string& message);
    std::string what() const;

public:
    std::string getModule() const;
    std::string getFileName() const;
    std::string getMessage() const;
};

inline Exception::Exception(
    const std::string& module,
    const std::string& file,
    const std::string& message)
    : m_module(module), m_file(file), m_message(message)
{}

inline std::string Exception::what() const
{
    std::stringstream ss;
    ss << m_module << " error";
    if (m_file.empty() == false)
        ss << " [" << m_file << "]";
    ss << ": " << m_message;
    return ss.str();
}

inline std::string Exception::getModule() const
{
    return m_module;
}

inline std::string Exception::getFileName() const
{
    return m_file;
}

inline std::string Exception::getMessage() const
{
    return m_message;
}

////////////////////////////////////////////////////////////////////////////////

class FormatException : public Exception
{
public:
    FormatException(const std::string& module,
        const std::string& file, const std::string& message);
};

inline FormatException::FormatException(const std::string& module,
    const std::string& file, const std::string& message)
    : Exception(module, file, message)
{}

////////////////////////////////////////////////////////////////////////////////

class IOException : public Exception
{
public:
    IOException(const std::string& module,
        const std::string& file, const std::string& message);
};

inline IOException::IOException(const std::string& module,
    const std::string& file, const std::string& message)
    : Exception(module, file, message)
{}

////////////////////////////////////////////////////////////////////////////////

class UnsupportedCamException : public Exception
{
public:
    UnsupportedCamException(
        const std::string &module, const std::string &message);
};

inline UnsupportedCamException::UnsupportedCamException(
    const std::string &module, const std::string &message)
    : Exception(module, "", message)
{}
