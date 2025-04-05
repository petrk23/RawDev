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

#include "CamProfiles/CamProfile.hpp"
#include "Utils.hpp"

using namespace Color;
using Utils::linearInter;

/*
CCT Temperature interpolation table
*/
const WhiteBalance::Ruvt WhiteBalance::k_tempTable[] = {
    {  0, 0.18006, 0.26352, -0.24341},
    { 10, 0.18066, 0.26589, -0.25479},
    { 20, 0.18133, 0.26846, -0.26876},
    { 30, 0.18208, 0.27119, -0.28539},
    { 40, 0.18293, 0.27407, -0.30470},
    { 50, 0.18388, 0.27709, -0.32675},
    { 60, 0.18494, 0.28021, -0.35156},
    { 70, 0.18611, 0.28342, -0.37915},
    { 80, 0.18740, 0.28668, -0.40955},
    { 90, 0.18880, 0.28997, -0.44278},
    {100, 0.19032, 0.29326, -0.47888},
    {125, 0.19462, 0.30141, -0.58204},
    {150, 0.19962, 0.30921, -0.70471},
    {175, 0.20525, 0.31647, -0.84901},
    {200, 0.21142, 0.32312,  -1.0182},
    {225, 0.21807, 0.32909,  -1.2168},
    {250, 0.22511, 0.33439,  -1.4512},
    {275, 0.23247, 0.33904,  -1.7298},
    {300, 0.24010, 0.34308,  -2.0637},
    {325, 0.24702, 0.34655,  -2.4681},
    {350, 0.25591, 0.34951,  -2.9641},
    {375, 0.26400, 0.35200,  -3.5814},
    {400, 0.27218, 0.35407,  -4.3633},
    {425, 0.28039, 0.35577,  -5.3762},
    {450, 0.28863, 0.35714,  -6.7262},
    {475, 0.29685, 0.35823,  -8.5955},
    {500, 0.30505, 0.35907,  -11.324},
    {525, 0.31320, 0.35968,  -15.628},
    {550, 0.32129, 0.36011,  -23.325},
    {575, 0.32931, 0.36038,  -40.770},
    {600, 0.33724, 0.36051,  -116.45}
};

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

    // Find relative weight for linear interpolation.
    const double weight = (k_tempTable[index + 1].r - miredTemp)
        / (k_tempTable[index + 1].r - k_tempTable[index].r);

    // Interpolate the black body coordinates as uv color.
    CIEuv uvNeutral {
        linearInter(k_tempTable[index].u, k_tempTable[index + 1].u, weight),
        linearInter(k_tempTable[index].v, k_tempTable[index + 1].v, weight)
    };
    if (m_tint != 0) {
        applyTint(index, uvNeutral, weight);
    }

    // Covert uv to camera space
    return xyzTo<RGB64>(camProfile.getColorMatrix(), uvToXYZ(uvNeutral, 1.0));
}

int WhiteBalance::FindNearestTempIndex(const double miredTemp)
{
    int index = 0;

    // Search for line pair containing coordinate.
    for (; index < 30; index++) {
        if (miredTemp < k_tempTable[index + 1].r || index == 29) {
            break;
        }
    }
    assert(index >= 0 && index < 31);
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
    // Find vectors along slope for each line.
    double uu1 = 1.0;
    double vv1 = k_tempTable[index].t;
    double len1 = sqrt(1.0 + vv1 * vv1);
    uu1 /= len1;
    vv1 /= len1;

    double uu2 = 1.0;
    double vv2 = k_tempTable[index + 1].t;
    double len2 = sqrt(1.0 + vv2 * vv2);
    uu2 /= len2;
    vv2 /= len2;

    // Find vector from black body point.
    double uu3 = Utils::linearInter(uu1, uu2, weight);
    double vv3 = Utils::linearInter(vv1, vv2, weight);
    const double len3 = sqrt(uu3 * uu3 + vv3 * vv3);
    uu3 /= len3;
    vv3 /= len3;

    // Adjust coordinate along this vector.
    const double offset = m_tint * (1.0 / k_tintScale);
    uv.u += uu3 * offset; // Apply tint correction.
    uv.v += vv3 * offset;
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
        maxRef / refCameraNeutral.r,
        maxRef / refCameraNeutral.g,
        maxRef / refCameraNeutral.b
    };
}
