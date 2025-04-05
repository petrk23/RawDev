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

#include "pch.hpp"

#include "CamProfiles/EOS_5D/Cam5D.hpp"
#include "CamProfiles/EOS_5D2/Cam5D2.hpp"
#include "CamProfiles/EOS_5D3/Cam5D3.hpp"
#include "CamProfiles/EOS_5D4/Cam5D4.hpp"
#include "CamProfiles/EOS_5Ds/Cam5Ds.hpp"
#include "CamProfiles/EOS_5DsR/Cam5DsR.hpp"
#include "CamProfiles/EOS_6D/Cam6D.hpp"
#include "CamProfiles/EOS_6D2/Cam6D2.hpp"
#include "CamProfiles/EOS_7D/Cam7D.hpp"
#include "CamProfiles/EOS_7D2/Cam7D2.hpp"

#include "Color.hpp"
#include "Utils.hpp"
#include "WhiteBalance.hpp"

struct TempTestData {
    WhiteBalance wb;
    CamID camID;
    WhiteBalance::Scale sc;
};

// Test data calculated by the reference DNG implementation
constexpr TempTestData k_td[] = {
    {{5000,  10}, CamID::EOS_5D2,  {2.22345493145620443087, 1.0, 1.67875574542152672386}},
    {{7500,   0}, CamID::EOS_5D2,  {2.67354486337949914798, 1.0, 1.24829631647088556967}},
    {{2500,   5}, CamID::EOS_5D2,  {1.19722809491637138812, 1.0, 3.67272159045186530335}},
    {{5000,  75}, CamID::EOS_5D4,  {2.16193544330780973439, 1.0, 1.81328418463537732741}},
    {{7200,   0}, CamID::EOS_5D4,  {2.27424514605992067473, 1.0, 1.17836250975245349082}},
    {{5500, -20}, CamID::EOS_5D4,  {1.97627860536445898454, 1.0, 1.26130044578552102941}},
    {{2500,   0}, CamID::EOS_5D4,  {1.14492604871406711275, 1.0, 2.42005600454397251653}},
    {{4335,  -5}, CamID::EOS_5D4,  {1.79147339053725573876, 1.0, 1.49569170207947110818}},
    {{7074, -73}, CamID::EOS_7D,   {2.01894817908908219550, 1.0, 1.07964763339993186086}},
    {{7529,  67}, CamID::EOS_5D2,  {3.11704384736897388564, 1.0, 1.49640010493720732043}},
    {{3635,  94}, CamID::EOS_7D2,  {1.66352264842195030070, 1.0, 3.81761208139230800995}},
    {{7567, -56}, CamID::EOS_7D,   {2.13316889015143074104, 1.0, 1.09331449376256939487}},
    {{6162, -39}, CamID::EOS_7D2,  {1.99628781764410123678, 1.0, 1.29405349346603903982}},
    {{3487,   9}, CamID::EOS_7D2,  {1.50317323694506899123, 1.0, 2.13541814975875565707}},
    {{4392, -63}, CamID::EOS_5D3,  {1.66465161682035578394, 1.0, 1.38742964120038569931}},
    {{5734,  99}, CamID::EOS_6D,   {2.50659327139913301608, 1.0, 2.03028491904250607547}},
    {{7788, 100}, CamID::EOS_5D4,  {2.94581243525423852603, 1.0, 1.39056445915074156083}},
    {{7825,  94}, CamID::EOS_5D,   {3.34059330938035703440, 1.0, 1.42294062450596814351}},
    {{3788,  45}, CamID::EOS_5DsR, {1.82084738388266265119, 1.0, 2.69520599378244352096}},
    {{7853,  97}, CamID::EOS_5D3,  {3.02619676694812955731, 1.0, 1.54817302915174415112}},
    {{5427,  60}, CamID::EOS_5DsR, {2.46518038068947920038, 1.0, 1.96197812089694534343}},
    {{7002, -41}, CamID::EOS_7D2,  {2.08905661995426061850, 1.0, 1.23010976921552783736}},
    {{5109, -78}, CamID::EOS_7D2,  {1.72808173209466597875, 1.0, 1.25676385936608592253}},
    {{7579,  28}, CamID::EOS_5D2,  {2.86153324701970612765, 1.0, 1.33968673366543655767}},
    {{6961,  76}, CamID::EOS_6D2,  {2.85258989042976729422, 1.0, 1.62551696329263561935}},
    {{7798,   1}, CamID::EOS_5D2,  {2.71931905239784521555, 1.0, 1.22930807061056279750}},
    {{6279,  60}, CamID::EOS_5D4,  {2.41136666773531516128, 1.0, 1.45036282317879083870}},
    {{3178, -28}, CamID::EOS_7D,   {1.39037046845502065828, 1.0, 1.84786064917131587215}},
    {{7246, -58}, CamID::EOS_6D,   {1.98095105648444991253, 1.0, 1.15700571807511409794}},
    {{7670,  36}, CamID::EOS_6D2,  {2.70429720627114944520, 1.0, 1.40499145262360602970}},
    {{6394, -20}, CamID::EOS_5D2,  {2.40175384492213650489, 1.0, 1.27647446716051238269}},
    {{6789,  48}, CamID::EOS_5Ds,  {2.75433292040048982585, 1.0, 1.61785066178035963347}}
};

////////////////////////////////////////////////////////////////////////////////

TEST(WhiteBalanceTest, ConstructionTest)
{
    constexpr WhiteBalance wb(6504, 7);
    EXPECT_DOUBLE_EQ(wb.getColorTemperature(), 6504);
    EXPECT_EQ(wb.getTint(), 7);
}

TEST(WhiteBalanceTest, ScaleTest)
{
    for (const auto& ti : k_td) {
        const auto camProfile =
            CamProfile::MakeCamProfile(ti.camID, ti.wb.getColorTemperature());
        EXPECT_EQ(ti.camID, camProfile->getCameraID());

        // Calculate the target scales
        const WhiteBalance::Scale sc = ti.wb.calcScales(*camProfile);

        // Compare with the ref
        constexpr double tolerance = 1e-12;
        EXPECT_NEAR(ti.sc.r, sc.r, tolerance);
        EXPECT_NEAR(ti.sc.g, sc.g, tolerance);
        EXPECT_NEAR(ti.sc.b, sc.b, tolerance);
        EXPECT_GE(sc.r, 0.0);
        EXPECT_GE(sc.g, 0.0);
        EXPECT_GE(sc.b, 0.0);
    }
}
