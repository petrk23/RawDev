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

#include <cctype>
#include <stdexcept>
#include <string>

class ArtistNameValidationException : public std::invalid_argument {
    int m_pos;

public:
    ArtistNameValidationException(int pos, const std::string& message);
    [[nodiscard]] int getPosition() const noexcept;
};

class ArtistNameValidator {
    enum class State { Begin, Name, AfterSpace };
    static constexpr size_t k_maxLenght = 64;

public:
    void operator()(const std::string& artistName);

private:
    static void CheckArtistName(const std::string& name);
    static void ArtistNameTooLong(const std::string& name);

    static bool IsInterpunction(char c) noexcept;
    static bool IsAlphaNumeric(char c);

    static void HandleBeginState(char c, State& state, int pos);
    static void HandleNameState(char c, State& state, int pos);
    static void HandleAfterSpaceState(char c, State& state, int pos);
};

////////////////////////////////////////////////////////////////////////////////

inline ArtistNameValidationException::ArtistNameValidationException(
    int pos, const std::string& message)
    : invalid_argument(message), m_pos(pos)
{
}

inline int ArtistNameValidationException::getPosition() const noexcept
{
    return m_pos;
}

inline bool ArtistNameValidator::IsInterpunction(char c) noexcept
{
    return c == '.' ||
           c == ',' ||
           c == ':' ||
           c == ';' ||
           c == '!' ||
           c == '?' ||
           c == '-';
}

inline bool ArtistNameValidator::IsAlphaNumeric(char c)
{
    return static_cast<bool>(std::isalnum(static_cast<unsigned char>(c)));
}

inline void ArtistNameValidator::HandleBeginState(char c, State& state, int pos)
{
    if (IsAlphaNumeric(c)) {
        state = State::Name;
        return;
    }
    throw ArtistNameValidationException(
        pos, "Artist name must begin with alphanumeric character.");
}

inline void ArtistNameValidator::HandleNameState(char c, State& state, int pos)
{
    if (c == ' ') {
        state = State::AfterSpace;
    }
    else if (!IsAlphaNumeric(c) && !IsInterpunction(c)) {
        throw ArtistNameValidationException(
            pos, "Character not allowed inside artist name.");
    }
}

inline void
ArtistNameValidator::HandleAfterSpaceState(char c, State& state, int pos)
{
    if (IsAlphaNumeric(c) || IsInterpunction(c)) {
        state = State::Name;
    }
    else if (c == ' ') {
        throw ArtistNameValidationException(
            pos, "Multiple consecutive spaces are not allowed.");
    }
    else {
        throw ArtistNameValidationException(
            pos, "Character not allowed after space.");
    }
}
