/*
 * This file is part of RawDev.
 *
 * Copyright (C) 2020-2023 Petr Krajn�k
 *
 * RawDev is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RawDev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RawDev.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

enum ColorProfile : int
{
	sRGB = 0, aRGB
};

const char * const kColorProfileNames[] = {
	"sRGB", "AdobeRGB(1998)"
};
