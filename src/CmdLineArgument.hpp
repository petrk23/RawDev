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
#include <cassert>

namespace CmdLine
{
    class Argument
    {
    public:
        Argument(const std::string &valueName, const std::string &descr,
            OptionType type, size_t groupID);
        Argument(const std::string &descr, bool invertible, size_t groupID);

    public:
        OptionType getOptionType() const;
        size_t getUsageCount() const;
        std::string getValueName() const;
        std::string getDescription() const;
        size_t getGroupID() const;
        int getPosition() const;
        bool isOption() const;
        bool isSwitch() const;
        bool isFound() const;

    public: // Value methods
        std::string getStringValue() const;
        int getIntValue() const;
        double getDoubleValue() const;
        void setStringValue(const std::string &value, int position);
        void setIntValue(int value, int position);
        void setDoubleValue(double value, int position);
        bool getSwitchState() const;
        void invert(int position); // Turn the switch

    private: // Argument data
        size_t m_UsageCount, m_GroupID;
        std::string m_Description;
        bool m_IsOption;
        int m_Position;

    private: // Option values
        OptionType m_OptionType;
        std::string m_ValueName;
        std::string m_StrVal;
        int m_IntVal;
        double m_DblVal;

    private: // Switch values
        bool m_IsInvertible;
    };
};

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Create Switch argument
/// </summary>
/// <param name="descr">Argument description</param>
/// <param name="invertible">Switch inversibility</param>
inline CmdLine::Argument::Argument(
    const std::string& descr, bool invertible, size_t groupID)
    : m_UsageCount(0), m_GroupID(groupID)
    , m_Description(descr), m_IsOption(false), m_Position(0)
    , m_OptionType(CmdLine::OptionType::STRING), m_ValueName()
    , m_StrVal(), m_IntVal(0), m_DblVal(0.0)
    , m_IsInvertible(invertible)
{}

/// <summary>
/// Create Option argument
/// </summary>
/// <param name="descr">Argument description</param>
/// <param name="type">Type of option value</param>
inline CmdLine::Argument::Argument(
    const std::string& valueName, const std::string &descr,
    OptionType type, size_t groupID)
    : m_UsageCount(0), m_GroupID(groupID)
    , m_Description(descr), m_IsOption(true), m_Position(0)
    , m_OptionType(type), m_ValueName(valueName)
    , m_StrVal(), m_IntVal(0), m_DblVal(0.0)
    , m_IsInvertible(false)
{}

inline CmdLine::OptionType CmdLine::Argument::getOptionType() const
{
    assert(isOption() == true);
    return m_OptionType;
}

inline size_t CmdLine::Argument::getUsageCount() const
{
    return m_UsageCount;
}

inline std::string CmdLine::Argument::getValueName() const
{
    assert(isOption() == true);
    return m_ValueName;
}

inline std::string CmdLine::Argument::getDescription() const
{
    return m_Description;
}

inline size_t CmdLine::Argument::getGroupID() const
{
    return m_GroupID;
}

inline int CmdLine::Argument::getPosition() const
{
    return m_Position;
}

inline bool CmdLine::Argument::isOption() const
{
    return m_IsOption;
}

inline bool CmdLine::Argument::isSwitch() const
{
    return !isOption();
}

inline bool CmdLine::Argument::isFound() const
{
    return m_UsageCount > 0;
}

inline std::string CmdLine::Argument::getStringValue() const
{
    assert(m_OptionType == OptionType::STRING);
    return m_StrVal;
}

inline int CmdLine::Argument::getIntValue() const
{
    assert(m_OptionType == OptionType::INT);
    return m_IntVal;
}

inline double CmdLine::Argument::getDoubleValue() const
{
    assert(m_OptionType == OptionType::DOUBLE);
    return m_DblVal;
}

inline void CmdLine::Argument::setStringValue(
    const std::string &value, int position)
{
    assert(m_OptionType == OptionType::STRING);
    m_StrVal = value;
    m_UsageCount++;
    m_Position = position;
    return;
}

inline void CmdLine::Argument::setIntValue(
    int value, int position)
{
    assert(m_OptionType == OptionType::INT);
    m_IntVal = value;
    m_UsageCount++;
    m_Position = position;
    return;
}

inline void CmdLine::Argument::setDoubleValue(
    double value, int position)
{
    assert(m_OptionType == OptionType::DOUBLE);
    m_DblVal = value;
    m_UsageCount++;
    m_Position = position;
    return;
}

inline bool CmdLine::Argument::getSwitchState() const
{
    assert(isSwitch() == true);
    if (m_IsInvertible == true)
        return getUsageCount() & 1;
    return getUsageCount() > 0;
}

inline void CmdLine::Argument::invert(int position)
{
    assert(isSwitch() == true);
    m_UsageCount++;
    m_Position = position;
    return;
}
