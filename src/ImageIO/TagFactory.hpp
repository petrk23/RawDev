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

#include <fstream>
#include "TiffTag.hpp"

class TagFactory
{
public:
    TiffTag* create(std::ifstream& file, const TiffTag::RawTag& rtag);

private:
    TiffTag* createStringTag(std::ifstream& file, const TiffTag::RawTag& rtag);
    bool isValidStringTag(const TiffTag::RawTag& rtag);

    TiffTag* createByteTag(std::ifstream& file, const TiffTag::RawTag& rtag);

    TiffTag* createShortTag(std::ifstream& file, const TiffTag::RawTag& rtag);
    bool isValidShortTag(const TiffTag::RawTag& rtag);

    TiffTag* createLongTag(std::ifstream& file, const TiffTag::RawTag& rtag);
    bool isValidLongTag(const TiffTag::RawTag& rtag);

    TiffTag* createRationaTag(std::ifstream& file, const TiffTag::RawTag& rtag);


};
