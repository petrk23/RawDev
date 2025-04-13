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

namespace Color {
struct CIEuv;
struct RGB64;
} // namespace Color

class CamProfile;

class WhiteBalance {
public:
    struct Scale {
        double rs, gs, bs;
    };

    WhiteBalance(double colorTemp, int tint);

    [[nodiscard]] double getColorTemperature() const noexcept;
    [[nodiscard]] int getTint() const noexcept;
    [[nodiscard]] Scale calcScales(const CamProfile& camProfile) const;

private:
    [[nodiscard]] auto getRefCameraNeutral(const CamProfile& camProfile) const;
    [[nodiscard]] Scale calcWhiteBalance(const CamProfile& camProfile) const;
    [[nodiscard]] bool applyTintRequested() const;
    void applyTint(int index, Color::CIEuv& uv, double weight) const;

    [[nodiscard]] static int FindNearestTempIndex(double miredTemp);
    [[nodiscard]] static Scale
    InvertAndNormalizeScales(const Color::RGB64& refCameraNeutral);

    struct Muvt { // m is temperature in mireds
        double m, u, v, t;
    };
    static const Muvt k_tempMap[];

    // Tint scale to match DNG/ACR
    static constexpr double k_tintScale = -1.0/3000.0;

    double m_colorTemp;
    int m_tint;
};

////////////////////////////////////////////////////////////////////////////////

inline double WhiteBalance::getColorTemperature() const noexcept
{
    return m_colorTemp;
}

inline int WhiteBalance::getTint() const noexcept
{
    return m_tint;
}

inline WhiteBalance::Scale
WhiteBalance::calcScales(const CamProfile& camProfile) const
{
    return calcWhiteBalance(camProfile);
}

inline bool WhiteBalance::applyTintRequested() const
{
    return m_tint != 0;
}
