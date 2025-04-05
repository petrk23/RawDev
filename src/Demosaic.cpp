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

#include "Demosaic.hpp"

#include "Options.hpp"
#include "StopWatch.hpp"
#include "Logger.hpp"
#include "RawDev.hpp"

#include "Demosaic/AlgorithmType.hpp"
#include "Demosaic/Bilinear.hpp"
#include "Demosaic/Freeman.hpp"
#include "Demosaic/HQLinear.hpp"
#include "Demosaic/AHD.hpp"

#include <iostream>
using namespace std;

/// <summary>
/// Demosaic module construction
/// </summary>
/// <param name="opt">Processing options</param>
DemosaicModule::DemosaicModule(const Options &opt)
{
    switch (opt.getDemosaicAlg())
    {
    case Demosaic::AlgorithmType::Bilinear:
        m_Algorithm = make_shared<Demosaic::Bilinear>();
        break;
    case Demosaic::AlgorithmType::Freeman:
        m_Algorithm = make_shared<Demosaic::Freeman>(opt.getDemosaicIter());
        break;
    case Demosaic::AlgorithmType::HQLinear:
        m_Algorithm = make_shared<Demosaic::HQLinear>();
        break;
    default: // AlgorithmType::AHD
        m_Algorithm = make_shared<Demosaic::AHD>();
        break;
    }
    return;
}

/// <summary>
/// Run demosaicing module
/// </summary>
/// <param name="img">Image to be demosaiced</param>
/// <param name="opt">Processing options</param>
void DemosaicModule::run(Image &img, const Options &opt)
{
    StopWatch watch(true);

    DemosaicModule demosaic(opt);
    demosaic.printLogo(RawDev::verbout);
    RawDev::verbout.newline();
    demosaic.process(img);

    watch.stop(); // Measuring time of demosaicing
    RawDev::verbout << "Demosaicing took " << watch << endl;
    return;
}

/// <summary>
/// Print demosaic algorithm logo message
/// </summary>
/// <param name="os">Stream for print</param>
void DemosaicModule::printLogo(Logger &os) const
{
    m_Algorithm->printLogo(os);
    return;
}

/// <summary>
/// Do the selected demosaicing
/// </summary>
/// <param name="img">Image to be demosaiced</param>
inline void DemosaicModule::process(Image &img)
{
    m_Algorithm->demosaic(img);
    return;
}
