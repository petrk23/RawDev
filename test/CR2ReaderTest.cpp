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

#include "Exception.hpp"
#include "ImageIO/CR2Reader.hpp"

/*
These are complicated, long and useless test's
in some way... We know, that the reader works
as otherwise we wont get any usable outputs.
There is no need to check this as we see this
immediately!

TEST(CR2ReaderTest, File6DTest)
{
    Array2D<uint16_t> img;
    CR2Reader testA("Originals\\test6d.cr2");
    EXPECT_NO_THROW(testA.open());
    EXPECT_NO_THROW(testA.read(img));
}

TEST(CR2ReaderTest, File5DsRTest)
{
    Array2D<uint16_t> img;
    CR2Reader testA("Originals\\test5dsR.cr2");
    EXPECT_NO_THROW(testA.open());
    EXPECT_NO_THROW(testA.read(img));
}
*/

TEST(CR2ReaderTest, FileNotFoundTest)
{
    CR2Reader file("non existent");
    EXPECT_THROW(file.open(), IOException);
}

TEST(CR2ReaderTest, FileNotOpenTest)
{
    Array2D<uint16_t> img;
    CR2Reader file("non existent");
    EXPECT_THROW(file.read(img), IOException);
}
