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

#include "WhiteBalance.hpp"

#include <cmath>
#include <ranges>
#include <stdexcept>

#include "CamProfiles/CamProfile.hpp"
#include "Utils.hpp"

using namespace Color;
using Utils::linearInter;

// CCT Temperature interpolation map
// clang-format off
const WhiteBalance::Muvt WhiteBalance::k_tempMap[] = {
    {.m =   0, .u = 0.18006, .v = 0.26352, .t = -0.24341},
    {.m =  10, .u = 0.18066, .v = 0.26589, .t = -0.25479},
    {.m =  20, .u = 0.18133, .v = 0.26846, .t = -0.26876},
    {.m =  30, .u = 0.18208, .v = 0.27119, .t = -0.28539},
    {.m =  40, .u = 0.18293, .v = 0.27407, .t = -0.30470},
    {.m =  50, .u = 0.18388, .v = 0.27709, .t = -0.32675},
    {.m =  60, .u = 0.18494, .v = 0.28021, .t = -0.35156},
    {.m =  70, .u = 0.18611, .v = 0.28342, .t = -0.37915},
    {.m =  80, .u = 0.18740, .v = 0.28668, .t = -0.40955},
    {.m =  90, .u = 0.18880, .v = 0.28997, .t = -0.44278},
    {.m = 100, .u = 0.19032, .v = 0.29326, .t = -0.47888},
    {.m = 125, .u = 0.19462, .v = 0.30141, .t = -0.58204},
    {.m = 150, .u = 0.19962, .v = 0.30921, .t = -0.70471},
    {.m = 175, .u = 0.20525, .v = 0.31647, .t = -0.84901},
    {.m = 200, .u = 0.21142, .v = 0.32312, .t = -1.01820},
    {.m = 225, .u = 0.21807, .v = 0.32909, .t = -1.21680},
    {.m = 250, .u = 0.22511, .v = 0.33439, .t = -1.45120},
    {.m = 275, .u = 0.23247, .v = 0.33904, .t = -1.72980},
    {.m = 300, .u = 0.24010, .v = 0.34308, .t = -2.06370},
    {.m = 325, .u = 0.24702, .v = 0.34655, .t = -2.46810},
    {.m = 350, .u = 0.25591, .v = 0.34951, .t = -2.96410},
    {.m = 375, .u = 0.26400, .v = 0.35200, .t = -3.58140},
    {.m = 400, .u = 0.27218, .v = 0.35407, .t = -4.36330},
    {.m = 425, .u = 0.28039, .v = 0.35577, .t = -5.37620},
    {.m = 450, .u = 0.28863, .v = 0.35714, .t = -6.72620},
    {.m = 475, .u = 0.29685, .v = 0.35823, .t = -8.59550},
    {.m = 500, .u = 0.30505, .v = 0.35907, .t = -11.3240},
    {.m = 525, .u = 0.31320, .v = 0.35968, .t = -15.6280},
    {.m = 550, .u = 0.32129, .v = 0.36011, .t = -23.3250},
    {.m = 575, .u = 0.32931, .v = 0.36038, .t = -40.7700},
    {.m = 600, .u = 0.33724, .v = 0.36051, .t = -116.450}
};
// clang-format on

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
WhiteBalance::WhiteBalance(double colorTemp, int tint)
    : m_colorTemp(colorTemp), m_tint(tint)
{
    if (std::isnan(m_colorTemp)) {
        throw std::invalid_argument("color temperature is NAN");
    }
    if (m_colorTemp < 2000.0 || m_colorTemp > 50000.0) {
        throw std::invalid_argument("color temperature out of range");
    }
    if (m_tint < -150 || m_tint > 150) {
        throw std::invalid_argument("tint out of range");
    }
}

/*
Calculates camera space white (neutral) from color temperature
and tint. The temperature results are roughly the same, but the
tint is much better, as its calculated together in the correct way.

This code is based on the Adobe DNG SDK version. As I liked it,
so why reinvent the wheel when we have such a good solution.

See "dng_temperature.cpp" for the original in the DNG SDK.
*/
auto WhiteBalance::getRefCameraNeutral(const CamProfile& camProfile) const
{
    const double miredTemp = kelvin2mired(m_colorTemp);
    const int index = FindNearestTempIndex(miredTemp);

    // Find relative weight for linear interpolation
    const double weight = (k_tempMap[index + 1].m - miredTemp) /
                          (k_tempMap[index + 1].m - k_tempMap[index].m);

    // Interpolate the black body coordinates as uv color
    CIEuv uvNeutral{
        .u = linearInter(k_tempMap[index].u, k_tempMap[index + 1].u, weight),
        .v = linearInter(k_tempMap[index].v, k_tempMap[index + 1].v, weight)};

    if (applyTintRequested()) {
        applyTint(index, uvNeutral, weight);
    }

    // Covert uv to camera space
    return xyzTo<RGB64>(camProfile.getColorMatrix(), uvToXYZ(uvNeutral, 1.0));
}

int WhiteBalance::FindNearestTempIndex(const double miredTemp)
{
    int index = -1;

    for (const auto& mapValue : k_tempMap | std::views::drop(1)) {
        ++index;
        if (miredTemp < mapValue.m) [[unlikely]] {
            break;
        }
    }
    return index;
}

/*
Calculates orthogonal vector to the Planckian locus (isotherm)
and moves values along it. This is the only correct way to handle
tint correction!
*/
void WhiteBalance::applyTint(
    const int index, CIEuv& uv, const double weight) const
{
    // Find vectors along slope for each line
    double uu1 = 1.0;
    double vv1 = k_tempMap[index].t;
    double len1 = hypot(1.0, vv1);
    uu1 /= len1;
    vv1 /= len1;

    double uu2 = 1.0;
    double vv2 = k_tempMap[index + 1].t;
    double len2 = hypot(1.0, vv2);
    uu2 /= len2;
    vv2 /= len2;

    // Find vector from black body point
    double uu3 = linearInter(uu1, uu2, weight);
    double vv3 = linearInter(vv1, vv2, weight);
    const double len3 = hypot(uu3, vv3);
    uu3 /= len3;
    vv3 /= len3;

    // Adjust coordinate along this vector
    const double offset = m_tint * k_tintScale;
    uv.u = fma(uu3, offset, uv.u); // Apply tint correction
    uv.v = fma(vv3, offset, uv.v);
}

WhiteBalance::Scale
WhiteBalance::calcWhiteBalance(const CamProfile& camProfile) const
{
    const auto refCameraNeutral = getRefCameraNeutral(camProfile);
    return InvertAndNormalizeScales(refCameraNeutral);
}

WhiteBalance::Scale
WhiteBalance::InvertAndNormalizeScales(const RGB64& refCameraNeutral)
{
    const auto maxRef = Utils::max3(
        refCameraNeutral.r, refCameraNeutral.g, refCameraNeutral.b);
    return {
        .rs = maxRef / refCameraNeutral.r,
        .gs = maxRef / refCameraNeutral.g,
        .bs = maxRef / refCameraNeutral.b};
}
