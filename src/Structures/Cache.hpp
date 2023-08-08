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

/*
 How RAW converters use cache
------------------------------
The process of developing a RAW image consists
from may different steps done in sequential manner.
Some of these steps are computationally very intensive.

Why do not insert checkpoints and save that state.
At some parameter change we don't need todo all the steps
again, but we can start at the nearest cached checkpoint state.

ACR cache works exact this way.

In this case it's not so important as we don't
need real time response when changing some parameter.
If we had GUI with sliders, as ACR has, implementation
of this feature would be necessarily.

Here there is no reason to do it. We don't need quick
responses on parameter change. The program speed would
be increased. Maybe.

This is not necessarily for the program function,
so I decided not to implement it. But I thing it is
important enough to leave this notice here!
*/
