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

#include "TimeUtils.hpp"

std::chrono::local_seconds TimeUtils::localNow()
{
    const auto now = std::chrono::floor<std::chrono::seconds>(
        std::chrono::system_clock::now());
    std::chrono::zoned_time zoned;

    try {
        zoned = std::chrono::zoned_time{std::chrono::current_zone(), now};
    }
    catch (const std::runtime_error&) {
        zoned = std::chrono::zoned_time{now}; // Fallback to UTC
    }
    return zoned.get_local_time();
}
