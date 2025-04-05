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

#include "ArtistNameValidator.hpp"

#include <cassert>
#include <sstream>

using std::string;

void ArtistNameValidator::operator()(const string& artistName)
{
    const auto nameLength = artistName.length();

    if (nameLength <= 0) {
        return; // Empty is ok...
    }
    if (nameLength >= k_maxLenght) {
        ArtistNameTooLong(artistName);
    }
    CheckArtistName(artistName);
}

void ArtistNameValidator::CheckArtistName(const std::string& name)
{
    auto state = State::Begin;
    const auto eci = name.cend();
    int pos = 1;

    for (auto ci = name.cbegin(); ci != eci; ++ci, ++pos) {
        switch (state) {
        case State::Name:
            HandleNameState(*ci, state, pos);
            break;
        case State::AfterSpace:
            HandleAfterSpaceState(*ci, state, pos);
            break;
        default: // State::Begin
            HandleBeginState(*ci, state, pos);
            break;
        }
    }
    if (state != State::Name) {
        throw ArtistNameValidationException(
            pos - 1, "Trailing spaces are not allowed.");
    }
}

inline void ArtistNameValidator::ArtistNameTooLong(const std::string& name)
{
    std::stringstream ss;
    ss << "Artist name is too long. "
        << name.length() << "/" << k_maxLenght << "max.";
    throw ArtistNameValidationException(-1, ss.str());
}
