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
#include <vector>
#include <string>
#include <map>
#include <cassert>

namespace CmdLine
{
    class Argument; // Forward declaration

    enum class OptionType
    {
        INT, DOUBLE, STRING
    };

    class Parser
    {
        // Argument map
        typedef std::map<std::string, Argument *> ArgumentMap;
        ArgumentMap m_Arguments;

        // Parameter list
        std::vector<std::string> m_ParamList;

        // Group list (argument grouping)
        std::vector<std::string> m_GroupList;

        // Input argument data
        const char **m_argv;
        int m_argc; // Argument count

        // Special function characters
        static const char k_SwitchChar, k_AssignChar;
        char m_SwitchChar, m_AssignChar;

    public: // Main interface
        Parser(); ~Parser();
        int parse(int argc, const char *argv[]);
        void setSwitchChar(char switchChar);
        void setAssignChar(char assignChar);

    public: // Add interface
        void addGroup(const std::string &descr);
        void addOption(const std::string &name, const std::string &valueName,
            const std::string &descr, OptionType type);
        void addSwitch(const std::string &name,
            const std::string &descr, bool invertible);

    public: // Found interface
        int found(const std::string &name) const;
        int found(const std::string &name, int &val) const;
        int found(const std::string &name, double &val) const;
        int found(const std::string &name, std::string &val) const;
        bool foundSwitch(const std::string &name) const;

    public: // Params interface
        size_t getParamCount() const;
        std::string getParam(size_t index) const;

    public: // Usage
        std::string getUsage(
            const std::string &cmdName, const std::string &paramName) const;
        static void error(int index, int pos, const std::string &msg);
        static void error(const std::string& msg);

    private: // Parser helpers
        int parseArgumentName(int &index);
        int parseOptionSingle(int index,
            const std::string &argName, const std::string &argValue);
        int parseOptionValue(int argIndex, Argument &arg,
            int valIndex, const std::string &argValue);
        static int parseIntValue(int argIndex, Argument &arg,
            int valIndex, const std::string &argValue);
        static int parseDblValue(int argIndex, Argument &arg,
            int valIndex, const std::string &argValue);
        int parseOptionOrSwitch(int &index, const std::string &argName);

    private: // Usage helpers
        void printCmdUsage(std::ostream &os,
            const std::string &cmdName, const std::string &paramName) const;
        static void printCmdName(std::ostream &os, const std::string &cmdName);
        void printArgument(std::ostream &os,
            const std::string &name, const Argument &argInfo) const;
        void printOptionGroups(std::ostream &os) const;

    private: // Other helpers
        size_t groupMaxLen(size_t groupID) const;
        size_t getActualGroupID() const;
        void addNewArgument(const std::string& name, Argument *newArgumentPtr);
    };
};

////////////////////////////////////////////////////////////////////////////////

inline void CmdLine::Parser::error(const std::string& msg)
{
    error(-1, 0, msg); // Generic error message
}

inline size_t CmdLine::Parser::getParamCount() const
{
    return m_ParamList.size();
}

inline std::string CmdLine::Parser::getParam(size_t index) const
{
    if (index < getParamCount())
        return m_ParamList[index];
    return std::string();
}

inline void CmdLine::Parser::setSwitchChar(char switchChar)
{
    m_SwitchChar = switchChar;
    return;
}

inline void CmdLine::Parser::setAssignChar(char assignChar)
{
    m_AssignChar = assignChar;
    return;
}

inline void CmdLine::Parser::printCmdName(
    std::ostream &os, const std::string &cmdName)
{
    if (cmdName.empty() == false)
        os << cmdName;
    else
        os << "?AppName?";
    return;
}

inline size_t CmdLine::Parser::getActualGroupID() const
{
    assert(m_GroupList.size() > 0);
    return m_GroupList.size() - 1;
}

inline void CmdLine::Parser::addGroup(const std::string &descr)
{
    m_GroupList.push_back(descr);
    return;
}
