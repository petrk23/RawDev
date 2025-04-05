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

#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

#include "CmdLineParser.hpp"
#include "CmdLineArgument.hpp"

#ifdef _WIN32
    /// <summary>
    /// Switch mark char for Windows
    /// </summary>
    const char CmdLine::Parser::k_SwitchChar = '/';

    /// <summary>
    /// Assign char for Windows
    /// </summary>
    const char CmdLine::Parser::k_AssignChar = ':';
#else
    /// <summary>
    /// Switch mark char for Unix, Linux and Mac
    /// </summary>
    const char CmdLine::Parser::k_SwitchChar = '-';

    /// <summary>
    /// Assign char for Unix, Linux and Mac
    /// </summary>
    const char CmdLine::Parser::k_AssignChar = '=';
#endif

/// <summary>
/// Construction of empty parser
/// </summary>
CmdLine::Parser::Parser()
    : m_Arguments(), m_ParamList(), m_GroupList()
    , m_argv(nullptr), m_argc(0)
    , m_SwitchChar(k_SwitchChar), m_AssignChar(k_AssignChar)
{
    addGroup("Options"); // Implicit group definition.
    return;
}

/// <summary>
/// Dealocate dynamic argument memory
/// </summary>
CmdLine::Parser::~Parser()
{
    for (auto &arg : m_Arguments)
    {
        assert(arg.second != nullptr);
        delete arg.second;
        arg.second = nullptr;
    }
    return;
}

/// <summary>
/// Parsing command line arguments
/// </summary>
/// <param name="argc">Argument count</param>
/// <param name="argv">Argument list</param>
/// <returns>Error count while processing</returns>
int CmdLine::Parser::parse(int argc, const char *argv[])
{
    m_argc = argc; // Save the cmd line into the class
    m_argv = argv; // to save function params!!!
    int errorCount = 0;

    for (int i = 1; i < m_argc; i++)
    {
        const char firstChar = m_argv[i][0];

        if (firstChar == '\0')
            continue;
        else if (firstChar == m_SwitchChar)
            errorCount += parseArgumentName(i);
        else
            m_ParamList.push_back(m_argv[i]);
    }
    return errorCount;
}

/// <summary>
/// Parsing argument name
/// </summary>
/// <param name="argIndex">Index of the processed argument</param>
/// <returns>Error count while processing the argument</returns>
int CmdLine::Parser::parseArgumentName(int &argIndex)
{
    string argName(m_argv[argIndex] + 1);
    const size_t assignPos = argName.find_first_of(m_AssignChar);
    int errorCount;

    // Is the value not in the same argument
    if (assignPos == string::npos)
    {
        errorCount = parseOptionOrSwitch(argIndex, argName);
    }
    else
    {
        string argValue(argName, assignPos + 1);
        argName.erase(assignPos); // Remove value part
        errorCount = parseOptionSingle(argIndex, argName, argValue);
    }
    return errorCount;
}

/// <summary>
/// Parsing option where value is in the same argument
/// </summary>
/// <param name="index">Argument index</param>
/// <param name="argName">Argument name</param>
/// <param name="argValue">Argument value</param>
/// <returns>Error count while processing</returns>
int CmdLine::Parser::parseOptionSingle(
    int index, const std::string &argName, const std::string &argValue)
{
    ArgumentMap::iterator it = m_Arguments.find(argName);
    if (it != m_Arguments.end() && it->second->isOption() == true)
    {
        // Option exist. Process the value...
        return parseOptionValue(index, *(it->second), index, argValue);
    }
    else
    {
        error(index, 1, "Option is unknown or it is not a option.");
    }
    return 1;
}

/// <summary>
/// Parse option or switch
/// </summary>
/// <param name="index">Argument index</param>
/// <param name="argName">Argument name</param>
/// <returns>Error count while processing</returns>
int CmdLine::Parser::parseOptionOrSwitch(int &index, const std::string &argName)
{
    ArgumentMap::iterator it = m_Arguments.find(argName);
    if (it != m_Arguments.end())
    {
        // Argument is a option
        if (it->second->isOption() == true)
        {
            if (index + 1 < m_argc)
            {
                index++; // Value is in the next argument
                return parseOptionValue(
                    index - 1, *(it->second), index, m_argv[index]);
            }
            else error(index, -1,
                "No value to parse. Unexpected end of commad line.");
        }
        else
        {
            assert(it->second->isSwitch() == true);
            it->second->invert(index); // Invert switch
            return 0;
        }
    }
    else
    {
        stringstream sstream;
        sstream << "Option with name '" << argName << "' is unknown.";
        error(index, 1, sstream.str());
    }
    return 1;
}

/// <summary>
/// Parsing option value
/// </summary>
/// <param name="index">Argument index</param>
/// <param name="arg">Argument descriptor</param>
/// <param name="argValue">Argument value to be parsed</param>
/// <returns>Error count while processing</returns>
int CmdLine::Parser::parseOptionValue(
    int argIndex, Argument &arg,
    int valIndex, const std::string &argValue)
{
    int errorCount = 0;

    switch (arg.getOptionType())
    {
    case OptionType::INT:
        errorCount = parseIntValue(argIndex, arg, valIndex, argValue);
        break;
    case OptionType::DOUBLE:
        errorCount = parseDblValue(argIndex, arg, valIndex, argValue);
        break;
    default: // CmdLineArgument::OptionType::STRING
        assert(arg.getOptionType() == OptionType::STRING);
        arg.setStringValue(argValue, argIndex);
        break;
    }
    return errorCount;
}

/// <summary>
/// Parsing integer value parameter
/// </summary>
/// <param name="index">Argument index</param>
/// <param name="arg">Argument descriptor</param>
/// <param name="argValue">Argument value to be parsed</param>
/// <returns>Error count while processing</returns>
int CmdLine::Parser::parseIntValue(
    int argIndex, Argument &arg,
    int valIndex, const std::string &argValue)
{
    const char *argValuePtr = argValue.c_str();
    char *endPtr; // Parsing end indicator
    int errorCount;

    int ival = strtol(argValuePtr, &endPtr, 10);
    if (argValuePtr == endPtr)
    {
        error(valIndex, 4, "Integer number expected.");
        errorCount = 1;
    }
    else if (*endPtr != '\0')
    {
        int pos = static_cast<int>(endPtr - argValuePtr) + 1;
        if (argIndex == valIndex)
            pos += 3; // If value is in one argument
        error(valIndex, pos, "Invalid residuum after the number value.");
        errorCount = 1;
    }
    else
    {
        assert(arg.getOptionType() == OptionType::INT);
        arg.setIntValue(ival, argIndex);
        errorCount = 0;
    }
    return errorCount;
}

/// <summary>
/// Parsing double value parameter
/// </summary>
/// <param name="index">Argument index</param>
/// <param name="arg">Argument descriptor</param>
/// <param name="argValue">Argument value to be parsed</param>
/// <returns>Error count while processing</returns>
int CmdLine::Parser::parseDblValue(
    int argIndex, Argument &arg,
    int valIndex, const std::string &argValue)
{
    const char *argValuePtr = argValue.c_str();
    char *endPtr; // Parsing end indicator
    int errorCount;

    double dval = strtod(argValuePtr, &endPtr);
    if (argValuePtr == endPtr)
    {
        error(valIndex, 4, "Double number expected.");
        errorCount = 1;
    }
    else if (*endPtr != '\0')
    {
        int pos = static_cast<int>(endPtr - argValuePtr) + 1;
        if (argIndex == valIndex)
            pos += 3; // If value is in one argument
        error(valIndex, pos, "Invalid residuum after the double value.");
        errorCount = 1;
    }
    else
    {
        assert(arg.getOptionType() == OptionType::DOUBLE);
        arg.setDoubleValue(dval, argIndex);
        errorCount = 0;
    }
    return errorCount;
}

/// <summary>
/// Print error message
/// </summary>
/// <param name="index">Argument index</param>
/// <param name="pos">Relative position inside argument</param>
/// <param name="msg">Error message to be printed</param>
void CmdLine::Parser::error(int index, int pos, const string &msg)
{
    cerr << "Error";
    if (index >= 0)
    {
        cerr << " [arg:" << index;
        if (pos >= 0)
            cerr << ", pos:" << pos;
        cerr << ']';
    }
    cerr << ": " << msg << endl;
    return;
}

/// <summary>
/// Find if the argument is found on the cmd line
/// </summary>
/// <param name="name">Name of the argument</param>
/// <returns>Argument index. If not found the zero.</returns>
int CmdLine::Parser::found(const std::string &name) const
{
    ArgumentMap::const_iterator it = m_Arguments.find(name);
    if (it != m_Arguments.end())
        return it->second->getPosition();
    return 0;
}

/// <summary>
/// Find and save integer option and his value
/// </summary>
/// <param name="name">Name of the integer option</param>
/// <param name="val">Value reference for output</param>
/// <returns>Argument index. If not found the zero.</returns>
int CmdLine::Parser::found(const std::string &name, int &val) const
{
    ArgumentMap::const_iterator it = m_Arguments.find(name);
    if (it != m_Arguments.end() &&
        it->second->isFound() == true &&
        it->second->isOption() == true &&
        it->second->getOptionType() == OptionType::INT)
    {
        val = it->second->getIntValue();
        return it->second->getPosition();
    }
    return 0;
}

/// <summary>
/// Find and save double option and his value
/// </summary>
/// <param name="name">Name of the double option</param>
/// <param name="val">Value reference for output</param>
/// <returns>Argument index. If not found the zero.</returns>
int CmdLine::Parser::found(const std::string &name, double &val) const
{
    ArgumentMap::const_iterator it = m_Arguments.find(name);
    if (it != m_Arguments.end() &&
        it->second->isFound() == true &&
        it->second->isOption() == true &&
        it->second->getOptionType() == OptionType::DOUBLE)
    {
        val = it->second->getDoubleValue();
        return it->second->getPosition();
    }
    return 0;
}

/// <summary>
/// Find and save string option and his value
/// </summary>
/// <param name="name">Name of the string option</param>
/// <param name="val">Value reference for output</param>
/// <returns>Argument index. If not found the zero.</returns>
int CmdLine::Parser::found(const std::string &name, std::string &val) const
{
    ArgumentMap::const_iterator it = m_Arguments.find(name);
    if (it != m_Arguments.end() &&
        it->second->isFound() == true &&
        it->second->isOption() == true &&
        it->second->getOptionType() == OptionType::STRING)
    {
        val = it->second->getStringValue();
        return it->second->getPosition();
    }
    return 0;
}

/// <summary>
/// Get switch state
/// </summary>
/// <param name="name">Name of the switch</param>
/// <returns>Boolean value of the switch</returns>
bool CmdLine::Parser::foundSwitch(const std::string &name) const
{
    ArgumentMap::const_iterator it = m_Arguments.find(name);
    if (it != m_Arguments.end() &&
        it->second->isFound() == true &&
        it->second->isSwitch() == true)
    {
        return it->second->getSwitchState();
    }
    return false;
}

/// <summary>
/// Add new option argument
/// </summary>
/// <param name="name">Option name</param>
/// <param name="descr">Option description</param>
/// <param name="type">Option type</param>
void CmdLine::Parser::addOption(
    const std::string &name, const std::string &valueName,
    const std::string &descr, OptionType type)
{
    if (name.empty() == false)
    {
        Argument *newOptionPtr = new Argument(
            valueName, descr, type, getActualGroupID());
        addNewArgument(name, newOptionPtr);
    }
    return;
}

/// <summary>
/// Add new swich argument
/// </summary>
/// <param name="name">Switch name</param>
/// <param name="descr">Switch description</param>
/// <param name="invertible">Switch inversibility</param>
void CmdLine::Parser::addSwitch(
    const std::string &name, const std::string &descr, bool invertible)
{
    if (name.empty() == false)
    {
        Argument *newSwitchPtr = new Argument(
            descr, invertible, getActualGroupID());
        addNewArgument(name, newSwitchPtr);
    }
    return;
}

/// <summary>
/// Format help and usage information
/// </summary>
/// <param name="cmdName"></param>
/// <param name="paramName"></param>
/// <returns></returns>
string CmdLine::Parser::getUsage(
    const string & cmdName, const string & paramName) const
{
    stringstream sstream;

    sstream << "Usage:" << endl;
    printCmdUsage(sstream, cmdName, paramName);
    printOptionGroups(sstream); // Print options in their groups
    sstream << "Option values can also be set in "
        << m_SwitchChar << "OPTION"
        << m_AssignChar << "value form."
        << endl
        << "Arguments can be mixed. There is no fixed order of usage."
        << endl;
    return sstream.str();
}

/// <summary>
/// Print cmd usage
/// </summary>
/// <param name="os">Output stream for printing</param>
/// <param name="cmdName">Cmd name as used on execution</param>
/// <param name="paramName">Program parameter name</param>
void CmdLine::Parser::printCmdUsage(std::ostream &os,
    const std::string &cmdName, const std::string &paramName) const
{
    printCmdName(os, cmdName); // Print cmd name
    size_t i = (groupMaxLen(0) > 0) ? 0 : 1;

    for (; i < m_GroupList.size(); i++)
    {
        os << " [" << m_GroupList[i] << "]";
    }
    if (paramName.empty() == false)
        os << " " << paramName << endl << endl;
    return;
}

/// <summary>
/// Print option in their groups
/// </summary>
/// <param name="os">Output stream for printing</param>
void CmdLine::Parser::printOptionGroups(std::ostream &os) const
{
    const size_t groupCount = m_GroupList.size();

    for (size_t i = 0; i < groupCount; i++)
    {
        const size_t maxlen = groupMaxLen(i);
        if (maxlen > 0) // Group is not empty
        {
            os << m_GroupList[i] << ":" << endl;
            for (auto &arg : m_Arguments)
            {
                const string &argName = arg.first;
                const Argument &argInfo = *(arg.second);

                if (argInfo.getGroupID() == i) {
                    os << "   " << setw(maxlen) << left;
                    printArgument(os, argName, argInfo);
                    os << "   " << argInfo.getDescription() << endl;
                }
            }
            os << endl;
        } // end if
    } // end for
    return;
}

/// <summary>
/// Print argument
/// </summary>
/// <param name="os">Output stream for printing</param>
/// <param name="name">Argument name</param>
/// <param name="argInfo">Argument info</param>
void CmdLine::Parser::printArgument(
    std::ostream &os, const std::string &name, const Argument &argInfo) const
{
    stringstream sstream;
    sstream << m_SwitchChar << name;
    if (argInfo.isOption() == true)
        sstream << ' ' << argInfo.getValueName();
    os << sstream.str();
    return;
}

/// <summary>
/// Calculate maximum argument len in a group
/// </summary>
/// <param name="groupID">Group id for counting</param>
/// <returns>Maximal len of argument of group with id</returns>
size_t CmdLine::Parser::groupMaxLen(size_t groupID) const
{
    size_t maxLen = 0;

    for (auto &arg : m_Arguments)
    {
        const Argument &argInfo = *(arg.second);
        if (argInfo.getGroupID() == groupID)
        {
            size_t len = arg.first.length() + 1;

            if (argInfo.isOption() == true)
                len += argInfo.getValueName().length() + 1;
            if (len > maxLen)
                maxLen = len;
        }
    }
    return maxLen;
}

/// <summary>
/// Add new argument to the list
/// </summary>
/// <param name="name">Argument name</param>
/// <param name="newArgumentPtr">Pointer to the new argument</param>
void CmdLine::Parser::addNewArgument(
    const std::string &name, Argument *newArgumentPtr)
{
    assert(name.empty() == false);
    assert(newArgumentPtr != nullptr);

    ArgumentMap::iterator it = m_Arguments.find(name);
    if (it != m_Arguments.end())
    {
        delete it->second;
        it->second = newArgumentPtr;
    }
    else
    {
        m_Arguments[name] = newArgumentPtr;
    }
    return;
}
