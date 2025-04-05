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
#include "ArtistNameValidator.hpp"

class ArtistNameValidatorTest : public ::testing::Test {
protected:
    ArtistNameValidator m_validator;
    void expectValidationException(const std::string& artistName, int pos);
};

// Assert exception with positional argument
void ArtistNameValidatorTest::expectValidationException(
    const std::string& artistName, int pos)
{
    try {
        m_validator(artistName);
        EXPECT_TRUE(false);
    }
    catch (const ArtistNameValidationException& ex) {
        EXPECT_EQ(pos, ex.getPosition());
    }
}

////////////////////////////////////////////////////////////////////////////////

TEST_F(ArtistNameValidatorTest, EmptyTest)
{
    m_validator(std::string()); // Empty is ok...
}

TEST_F(ArtistNameValidatorTest, RealNamesTest)
{
    m_validator("Bc. Petr Krajnik");
    m_validator("P.K.");
}

TEST_F(ArtistNameValidatorTest, SpaceTest)
{
    // Leading and trailing spaces
    expectValidationException(" xyz", 1);
    expectValidationException("abc ", 4);

    // Multispaces are not allowed
    expectValidationException("a  b  c", 3);

    m_validator("a b c"); // Correct spacing
}

TEST_F(ArtistNameValidatorTest, StartCharaterTest)
{
    m_validator("a");
    m_validator("a b");
    m_validator("1");
    m_validator("1 a");

    // Not alphanumeric first charater
    expectValidationException(";ab", 1);
}

TEST_F(ArtistNameValidatorTest, RepeatedInterpuctionTest)
{
    m_validator("A,.;:B?!");
    m_validator("A...B-C");
}

TEST_F(ArtistNameValidatorTest, InterpunctionAfterSpaceTest)
{
    m_validator("a :b");
}

TEST_F(ArtistNameValidatorTest, TooLongTest)
{
    const std::string longParam = std::string(
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    expectValidationException(longParam, -1);
}
