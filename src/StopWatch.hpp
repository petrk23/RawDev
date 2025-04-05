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

#include <chrono>
#include <ostream>

class StopWatch {
    using Clock = std::chrono::high_resolution_clock;
    using TimeVal = Clock::time_point;

    TimeVal m_start, m_stop;
    bool m_running;

public:
    explicit StopWatch(bool start = false);
    friend std::ostream& operator<<(std::ostream& os, const StopWatch& watch);

    void start();
    void stop();
    void reset();

    [[nodiscard]] auto currTime() const;
    [[nodiscard]] bool running() const;
};

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Construction of an empty stop watch
/// </summary>
inline StopWatch::StopWatch(bool startNow)
    : m_running(startNow)
{
    if (startNow) {
        start();
    }
}

/// <summary>
/// Start measuring time
/// </summary>
inline void StopWatch::start()
{
    m_start = Clock::now();
    m_running = true;
}

/// <summary>
/// Stop measuring time
/// </summary>
inline void StopWatch::stop()
{
    if (running()) {
        m_stop = Clock::now();
        m_running = false;
    }
}

/// <summary>
/// Reset stop watch to initial state
/// </summary>
inline void StopWatch::reset()
{
    m_start = TimeVal();
    m_stop = m_start;
    m_running = false;
}

/// <summary>
/// Get current stopwatch time reading
/// </summary>
/// <returns>Current time reading</returns>
inline auto StopWatch::currTime() const
{
    TimeVal stopVal;

    // Evaluate stopwatch time (continuous vs stoped)
    if (running()) {
        stopVal = Clock::now();
    }
    else {
        stopVal = m_stop;
    }
    // Duration value as double in seconds
    return std::chrono::duration<double>(stopVal - m_start).count();
}

/// <summary>
/// Get information whether the stopwatch is running
/// </summary>
/// <returns>True if stopwatch is running. Otherwise false</returns>
inline bool StopWatch::running() const
{
    return m_running;
}

/// <summary>
/// Print measured time from StopWatch
/// </summary>
/// <param name="os">Output stream for output</param>
/// <param name="watch">Reference to StopWatch object</param>
/// <returns>Stream reference for next usage</returns>
inline std::ostream& operator<<(std::ostream& os, const StopWatch& watch)
{
    auto durval = watch.currTime();
    constexpr decltype(durval) kONEMIN = 60.0; // 1min = 60s

    if (durval >= kONEMIN) {
        const auto min = static_cast<int>(durval / kONEMIN);
        durval -= kONEMIN * min; // Subtract minutes
        os << min << "m ";
    }
    const auto prec = os.precision(3);
    os << durval << "s";
    os.precision(prec);
    return os;
}
