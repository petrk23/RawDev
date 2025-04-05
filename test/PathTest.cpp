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
#include "Structures/Path.hpp"

static void TestSetExtension( // Extensive test extension setup
    const std::string& path, const std::string& name, char delim)
{
    std::string s = path + name;

    Path p1(s + ".tex", delim);
    EXPECT_EQ(p1.getPath(), std::string(s + ".tex"));
    EXPECT_EQ(p1.getFileName(), std::string(name + ".tex"));
    EXPECT_EQ(p1.getExtension(), std::string("tex"));

    p1.setExtension("tif"); // Change extension without dot
    EXPECT_EQ(p1.getPath(), std::string(s + ".tif"));
    EXPECT_EQ(p1.getFileName(), std::string(name + ".tif"));
    EXPECT_EQ(p1.getExtension(), std::string("tif"));

    p1.setExtension(".jpeg"); // Change extension with dot
    EXPECT_EQ(p1.getPath(), std::string(s + ".jpeg"));
    EXPECT_EQ(p1.getFileName(), std::string(name + ".jpeg"));
    EXPECT_EQ(p1.getExtension(), std::string("jpeg"));

    p1.setExtension("jpeg.tif.png"); // Multi extension without dot
    EXPECT_EQ(p1.getPath(), std::string(s + ".jpeg.tif.png"));
    EXPECT_EQ(p1.getFileName(), std::string(name + ".jpeg.tif.png"));
    EXPECT_EQ(p1.getExtension(), std::string("png"));

    p1.setExtension(".jpeg.tif.gif"); // Multi extension with dot
    EXPECT_EQ(p1.getPath(), std::string(s + ".jpeg.tif.jpeg.tif.gif"));
    EXPECT_EQ(p1.getFileName(), std::string(name + ".jpeg.tif.jpeg.tif.gif"));
    EXPECT_EQ(p1.getExtension(), std::string("gif"));

    // Empty extension
    Path p2(s + ".cpp", delim);
    EXPECT_EQ(p2.getPath(), std::string(s + ".cpp"));
    EXPECT_EQ(p2.getFileName(), std::string(name + ".cpp"));
    EXPECT_EQ(p2.getExtension(), std::string("cpp"));

    p2.setExtension(""); // Change extension empty
    EXPECT_EQ(p2.getPath(), std::string(s));
    EXPECT_EQ(p2.getPath(), s);
    EXPECT_EQ(p2.getExtension(), std::string(""));

    // Empty filename
    Path p3(path + ".test", delim);
    EXPECT_EQ(p3.getPath(), std::string(path + ".test"));
    EXPECT_EQ(p3.getFileName(), std::string(".test"));
    EXPECT_EQ(p3.getExtension(), std::string("test"));

    p3.setExtension("cosi");
    EXPECT_EQ(p3.getPath(), std::string(path + ".cosi"));
    EXPECT_EQ(p3.getFileName(), std::string(".cosi"));
    EXPECT_EQ(p3.getExtension(), std::string("cosi"));
}

TEST(PathTest, EmptyTest1)
{
    Path p;
    EXPECT_EQ(p.getExtension(), std::string());
    EXPECT_EQ(p.getFileName(), std::string());
}

TEST(PathTest, EmptyTest2)
{
    { // Test const char empty
        Path p("");
        EXPECT_EQ(p.getExtension(), std::string());
        EXPECT_EQ(p.getFileName(), std::string());
    }
    { // Test empty string
        Path p(std::string(""));
        EXPECT_EQ(p.getExtension(), std::string());
        EXPECT_EQ(p.getFileName(), std::string());
    }
}

TEST(PathTest, DotEndTest)
{
    Path p("testcosi.");
    EXPECT_EQ(p.getExtension(), std::string(""));
    EXPECT_EQ(p.getFileName(), std::string("testcosi."));
    return;
}

TEST(PathTest, DotBeginTest)
{
    Path p(".testcosi");
    EXPECT_EQ(p.getExtension(), std::string("testcosi"));
    EXPECT_EQ(p.getFileName(), p.getPath());
    return;
}

TEST(PathTest, MultiDotTest)
{
    Path p(".test.cosi.pdf");
    EXPECT_EQ(p.getExtension(), std::string("pdf"));
    EXPECT_EQ(p.getFileName(), std::string(".test.cosi.pdf"));
    return;
}

TEST(PathTest, NoExtensionTest)
{
    Path p("testcosipdf");
    EXPECT_EQ(p.getExtension(), std::string(""));
    EXPECT_EQ(p.getFileName(), std::string("testcosipdf"));
    return;
}

TEST(PathTest, DirPathTest)
{
    constexpr char winPathSep = '\\';

    Path dp1(R"(u:\x\y.z\ahoj)", winPathSep);
    EXPECT_EQ(dp1.getPath(), std::string(R"(u:\x\y.z\ahoj)"));
    EXPECT_EQ(dp1.getExtension(), std::string(""));
    EXPECT_EQ(dp1.getFileName(), std::string("ahoj"));

    Path dp2(R"(u:\x\y.z\ah.oj.tex)", winPathSep);
    EXPECT_EQ(dp2.getPath(), std::string(R"(u:\x\y.z\ah.oj.tex)"));
    EXPECT_EQ(dp2.getExtension(), std::string("tex"));
    EXPECT_EQ(dp2.getFileName(), std::string("ah.oj.tex"));
    return;
}

TEST(PathTest, UnixPathTest)
{
    Path dp1("u/x/y.z/ah.oj", '\\');
    EXPECT_EQ(dp1.getDirSeparator(), '\\');
    EXPECT_EQ(dp1.getExtension(), std::string("oj"));
    EXPECT_EQ(dp1.getFileName(), std::string("u/x/y.z/ah.oj"));

    dp1.setDirSeparator('/'); // Make unix paths
    EXPECT_EQ(dp1.getDirSeparator(), '/');
    EXPECT_EQ(dp1.getExtension(), std::string("oj"));
    EXPECT_EQ(dp1.getFileName(), std::string("ah.oj"));
    return;
}

TEST(PathTest, PathConversionTest)
{
    Path p("my test path");
    EXPECT_EQ(static_cast<std::string>(p), p.getPath());
    return;
}

TEST(PathTest, SetExtensionTest)
{
    TestSetExtension("", "ahoj", '\\');
    TestSetExtension(R"(u:\x\y.z\)", "ahoj", '\\');
    TestSetExtension("u/x/y.z/", "ahoj", '/'); // Unix style
}
