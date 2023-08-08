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
#pragma once

#include <ostream>
#include <ctime>

class StopWatch
{
	clock_t m_Start, m_Stop;
	bool m_Running;

public:
	explicit StopWatch(bool start = false);
	friend std::ostream &operator<< (std::ostream &, const StopWatch &);

public:
	void start();
	void stop();
	void reset();
	bool running() const;
};

////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Construction of an empty stop watch
/// </summary>
inline StopWatch::StopWatch(bool start)
	: m_Start(0), m_Stop(0), m_Running(start)
{
	if (running())
		m_Start = clock();
	return;
}

/// <summary>
/// Start measuring time
/// </summary>
inline void StopWatch::start()
{
	m_Start = clock();
	m_Running = true;
	return;
}

/// <summary>
/// Stop measuring time
/// </summary>
inline void StopWatch::stop()
{
	if (running())
	{
		m_Stop = clock();
		m_Running = false;
	}
	return;
}

/// <summary>
/// Reset stop watch to initial state
/// </summary>
inline void StopWatch::reset()
{
	m_Start = m_Stop = 0;
	m_Running = false;
	return;
}

/// <summary>
/// Get information whether the stopwatch is running
/// </summary>
/// <returns>True if stopwatch is running. Otherwise false</returns>
inline bool StopWatch::running() const
{
	return m_Running;
}

/// <summary>
/// Print measured time from StopWatch
/// </summary>
/// <param name="os">Output stream for output</param>
/// <param name="watch">Reference to StopWatch object</param>
/// <returns>Stream reference for next usage</returns>
inline std::ostream& operator<< (std::ostream& os, const StopWatch& watch)
{
	const clock_t tdiff = watch.m_Stop - watch.m_Start;
	double sec = static_cast<double>(tdiff) / CLOCKS_PER_SEC;
	if (sec >= 60.0) {
		const int min = static_cast<int>(sec / 60.0);
		sec -= 60.0 * min; // Subtract minutes
		os << min << "m ";
	}
	const std::streamsize prec = os.precision(3);
	os << sec << "s";
	os.precision(prec);
	return os;
}
