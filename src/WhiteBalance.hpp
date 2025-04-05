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

namespace Color { struct CIEuv; struct RGB64; }
class CamProfile;

class WhiteBalance {
    struct Ruvt { // r is temp. in mireds
        double r, u, v, t;
    };

    static const Ruvt k_tempTable[];
    static constexpr int k_tintScale = -3000; // Tint scale to match DNG/ACR

    double m_colorTemp;
    int m_tint;

public:
    struct Scale {
        double r, g, b;
    };

    constexpr WhiteBalance(double colorTemp, int tint);

    [[nodiscard]] constexpr double getColorTemperature() const;
    [[nodiscard]] constexpr int getTint() const;
    [[nodiscard]] Scale calcScales(const CamProfile& camProfile) const;

private:
    [[nodiscard]] auto getRefCameraNeutral(const CamProfile& camProfile) const;
    [[nodiscard]] Scale calcWhiteBalance(const CamProfile& camProfile) const;
    void applyTint(int index, Color::CIEuv& uv, double weight) const;
    [[nodiscard]] static int FindNearestTempIndex(double miredTemp);
    [[nodiscard]] static Scale
    InvertAndNormalizeScales(const Color::RGB64& refCameraNeutral);
};

////////////////////////////////////////////////////////////////////////////////

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr WhiteBalance::WhiteBalance(double colorTemp, int tint)
    : m_colorTemp(colorTemp), m_tint(tint)
{
}

constexpr double WhiteBalance::getColorTemperature() const
{
    return m_colorTemp;
}

constexpr int WhiteBalance::getTint() const
{
    return m_tint;
}

inline WhiteBalance::Scale
WhiteBalance::calcScales(const CamProfile& camProfile) const
{
    return calcWhiteBalance(camProfile);
}
