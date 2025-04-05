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

#include <memory>
#include <ostream>

namespace Demosaic { class IAlgorithm; };
class Options;
class Image;
class Logger;

class DemosaicModule
{
    std::shared_ptr<Demosaic::IAlgorithm> m_Algorithm;

public:
    static void run(Image &img, const Options &opt);
    void printLogo(Logger &os) const;

private:
    DemosaicModule(const Options &opt);
    void process(Image &img);
};
