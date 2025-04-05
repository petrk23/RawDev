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

#include "RawDev.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "Exception.hpp"

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Define the macro as 'throw' for tracing from where
// we get the exception by using a debugger.
#ifndef RAWDEV_DIAGNOSTIC_THROW
#define RAWDEV_DIAGNOSTIC_THROW
#endif

static int runRawDev(const int argc, const char* argv[])
{
    int exitCode = EXIT_FAILURE;

    try {
        RawDev app;
        exitCode = app.run(argc, argv);
    }
    catch (const Exception& ex) {
        std::cerr << ex.what() << std::endl;
        RAWDEV_DIAGNOSTIC_THROW;
    }
    catch (const std::bad_alloc& ex) {
        std::cerr << "Error: Failed to allocate memory."
                  << " (" << ex.what() << ")"
                  << std::endl;
        RAWDEV_DIAGNOSTIC_THROW;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        RAWDEV_DIAGNOSTIC_THROW;
    }
    catch (...) {
        std::cerr << "Unknown error occurred. CRASHING!" << std::endl;
        throw;
    }
    return exitCode;
}

int main(const int argc, const char* argv[])
{
    const int result = runRawDev(argc, argv);

#ifdef _WIN32
    _CrtDumpMemoryLeaks(); // Check for memory leaks
#endif
    return result;
}
