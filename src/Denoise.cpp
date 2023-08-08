/*
 * This file is part of RawDev.
 *
 * Copyright (C) 2020-2023 Petr Krajník
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
#include "Denoise.hpp"

#include "Structures/Image.hpp"
#include "Options.hpp"
#include "RawDev.hpp"

#include <iostream>
using namespace std;

void DenoiseModule::run(Image &img, const Options &opt)
{
	DenoiseModule denoise(opt);
	denoise.process(img);
	return;
}

void DenoiseModule::process(Image &)
{
	// TODO: Bayer mask denoising
	RawDev::verbout << "Denoising is not yet implemented." << endl;
	return;
}
