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

#include "pch.hpp"
#include "CmdLineParser.hpp"

// Test data setup 1
static const char* k_argv1[] = {
    "path", "/a", "/b:c", "/d", "1989", "f", "g"};
constexpr int k_argc1 = sizeof(k_argv1) / sizeof(char*);

// Test data setup 2
static const char* k_argv2[] = {
    "path", "-a", "-b=c", "-d", "1989", "f", "g"};
constexpr int k_argc2 = sizeof(k_argv2) / sizeof(char*);

// Test data setup 3 (errors)
// Note: Error message text's must be checked manually!
static const char* k_argv3[] = {
    "path",
    "-UknownSwitch",
    "-b:",         // No ERROR: Only empty string!
    "-i", "abc",   // No number
    "-i:",         // No number
    "-i:xxx",      // No number
    "-i", "12345", // Valid number
    "-i:123x",     // Residuum
    "-i", "1989x", // Residuum
    "-d", "xxx",   // Not number
    "-d:1.2345",   // Valid number
    "-d", "123x",  // Residuum
    "-d:",         // No value
    "-d:xxx",      // No number
    "-d:123x",     // Residuum
    "-d:",         // No number
    "-d"           // No value
};
constexpr int k_argc3 = sizeof(k_argv3) / sizeof(char*);

// Test data setup 4 (switches)
static const char* k_argv4[] = {
    "path", "-x", "-z", "-x", "-x", "-y", "-y", "-z"};
constexpr int k_argc4 = sizeof(k_argv4) / sizeof(char*);

////////////////////////////////////////////////////////////////////////////////

static void InitParser(CmdLine::Parser& parser)
{
    parser.addSwitch("a", "switch", true);
    parser.addOption("b", "str", "", CmdLine::OptionType::STRING);
    parser.addOption("d", "year", "", CmdLine::OptionType::INT);
}

static void TestCmdLine(const CmdLine::Parser& parser)
{
    // Test find only
    EXPECT_EQ(parser.found("a"), 1);
    EXPECT_EQ(parser.found("c"), 0);

    // Find options
    std::string sval;
    int ival;
    EXPECT_EQ(parser.found("b", sval), 2);
    EXPECT_EQ(sval, std::string("c"));
    EXPECT_EQ(parser.found("d", ival), 3);
    EXPECT_EQ(ival, 1989);

    // Test params
    EXPECT_EQ(parser.getParamCount(), static_cast<size_t>(2));
    EXPECT_EQ(parser.getParam(0), std::string("f"));
    EXPECT_EQ(parser.getParam(1), std::string("g"));
}

////////////////////////////////////////////////////////////////////////////////

TEST(CmdLineTest, FoundTest)
{
    CmdLine::Parser parser;
    parser.setSwitchChar('/'); // Windows like switches
    parser.setAssignChar(':');

    InitParser(parser);
    const int res = parser.parse(k_argc1, k_argv1);
    EXPECT_EQ(res, 0); // No syntax errors
    TestCmdLine(parser);
}

TEST(CmdLineTest, TestInvertibility)
{
    CmdLine::Parser parser;
    parser.addSwitch("x", "invertible x", true);
    parser.addSwitch("y", "invertible y", true);
    parser.addSwitch("z", "non invertible z", false);
    parser.setSwitchChar('-'); // Set specific chars

    const int res = parser.parse(k_argc4, k_argv4);
    EXPECT_EQ(0, res); // No syntax errors
    EXPECT_EQ(4, parser.found("x"));
    EXPECT_EQ(true, parser.foundSwitch("x"));
    EXPECT_EQ(6, parser.found("y"));
    EXPECT_EQ(false, parser.foundSwitch("y"));
    EXPECT_EQ(7, parser.found("z"));
    EXPECT_EQ(true, parser.foundSwitch("z"));
    EXPECT_EQ(static_cast<size_t>(0), parser.getParamCount());
}

TEST(CmdLineTest, TestSpecialChars)
{
    CmdLine::Parser parser;
    InitParser(parser);
    parser.setSwitchChar('-');
    parser.setAssignChar('=');

    const int res = parser.parse(k_argc2, k_argv2);
    EXPECT_EQ(res, 0); // No syntax errors
    TestCmdLine(parser);
}

TEST(CmdLineTest, TestErrorConditions)
{
    CmdLine::Parser parser;
    InitParser(parser);
    parser.addOption("i", "int", "", CmdLine::OptionType::INT);
    parser.addOption("d", "double", "", CmdLine::OptionType::DOUBLE);
    parser.setSwitchChar('-'); // Set specific chars
    parser.setAssignChar(':');

    const int res = parser.parse(k_argc3, k_argv3);
    EXPECT_EQ(13, res); // Exact expected number count

    // Test internal structure state when error occurs
    std::string bval; // Empty string special
    EXPECT_EQ(2, parser.found("b", bval));
    EXPECT_EQ(std::string(""), bval);
    int ival; // Value
    EXPECT_EQ(7, parser.found("i", ival));
    EXPECT_EQ(12345, ival);
    double dval;
    EXPECT_EQ(14, parser.found("d", dval));
    EXPECT_EQ(1.2345, dval);
    EXPECT_EQ(static_cast<size_t>(0), parser.getParamCount());
}

TEST(CmdLineTest, TestUsage)
{
    std::stringstream sstream;
    sstream << "Usage:\n"
            << "PRG [Options] FILEs" << std::endl
            << std::endl
            << "Options:" << std::endl
            << "   +a        switch" << std::endl
            << "   +b str    " << std::endl
            << "   +d year   " << std::endl
            << std::endl
            << "Option values can also be set in +OPTION%value form."
            << std::endl
            << "Arguments can be mixed. There is no fixed order of usage."
            << std::endl;

    CmdLine::Parser parser;
    InitParser(parser);
    parser.setSwitchChar('+');
    parser.setAssignChar('%');
    EXPECT_EQ(sstream.str(), parser.getUsage("PRG", "FILEs"));
}

TEST(CmdLineTest, TestUsageWithGroupsA)
{
    std::stringstream sstream;
    sstream << "Usage:\n"
            << "PRG [Group 1] [Group 2] FILEs" << std::endl
            << std::endl
            << "Group 1:" << std::endl
            << "   +a        switch" << std::endl
            << "   +b str    " << std::endl
            << "   +d year   " << std::endl
            << std::endl
            << "Group 2:" << std::endl
            << "   +x xval   my x val" << std::endl
            << std::endl
            << "Option values can also be set in +OPTION%value form."
            << std::endl
            << "Arguments can be mixed. There is no fixed order of usage."
            << std::endl;

    CmdLine::Parser parser;
    parser.addGroup("Group 1");
    InitParser(parser);
    parser.addGroup("Group 2");
    parser.addOption("x", "xval", "my x val", CmdLine::OptionType::INT);
    parser.setSwitchChar('+');
    parser.setAssignChar('%');
    EXPECT_EQ(sstream.str(), parser.getUsage("PRG", "FILEs"));
}

TEST(CmdLineTest, TestUsageWithGroupsB)
{
    std::stringstream sstream;
    sstream << "Usage:" << std::endl
            << "PRG [Options] [Group Non Default] FILEs"
            << std::endl
            << std::endl
            << "Options:" << std::endl
            << "   +a        switch" << std::endl
            << "   +b str    " << std::endl
            << "   +d year   " << std::endl
            << std::endl
            << "Group Non Default:" << std::endl
            << "   +x xval   my x val" << std::endl
            << std::endl
            << "Option values can also be set in +OPTION%value form."
            << std::endl
            << "Arguments can be mixed. There is no fixed order of usage."
            << std::endl;

    CmdLine::Parser parser;
    InitParser(parser); // Default group
    parser.addGroup("Group Non Default");
    parser.addOption("x", "xval", "my x val", CmdLine::OptionType::INT);
    parser.setSwitchChar('+');
    parser.setAssignChar('%');
    EXPECT_EQ(sstream.str(), parser.getUsage("PRG", "FILEs"));
}
