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

#include <cassert>
#include <memory>
#include <string_view>

#include "CamID.hpp"
#include "CamProfileHelpers.hpp"
#include "CFAPattern.hpp"
#include "Color.hpp"
#include "Structures/HSVMap.hpp"
#include "Structures/Mat3x3.hpp"
#include "Structures/Rect.hpp"
#include "Utils.hpp"

class CamProfile {
private:
    CFAPattern m_cfa = CFAPattern::Filter::RGGB;

    // Channel ranges
    Color::RGB64 m_black = {0, 0, 0};
    Color::RGB64 m_white = {0, 0, 0};
    double m_baseExposure = 0.0;

    // Sensor areas
    Rect m_activeArea;
    Rect m_crop; // Final image crop

    // Color temperature for interpolation
    double m_ctemp;

    double m_illu1 = 2856.0; // Illuminant A
    double m_illu2 = 6504.0; // Illuminant D65

    // Transform matrices
    Mat3x3 m_forwardMat;
    Mat3x3 m_colorMat;
    Mat3x3 m_analogBalance = Mat3x3::k_unitMatrix;

    // HSV Maps and profile look
    std::shared_ptr<HSVMap> m_hsvMap;
    std::shared_ptr<HSVMap> m_profileLook;

public:
    explicit CamProfile(double ctemp);
    virtual ~CamProfile() = default;

    // Geters
    [[nodiscard]] virtual std::string_view getCameraName() const = 0;
    [[nodiscard]] virtual CamID getCameraID() const = 0;
    [[nodiscard]] CFAPattern getCFAPattern() const;
    [[nodiscard]] Color::RGB64 getBlackLevel() const;
    [[nodiscard]] Color::RGB64 getWhiteLevel() const;
    [[nodiscard]] double getBaseExposure() const;
    [[nodiscard]] Rect getActiveArea() const;
    [[nodiscard]] Rect getCrop() const;
    [[nodiscard]] double getIllu1() const;
    [[nodiscard]] double getIllu2() const;
    [[nodiscard]] Mat3x3 getForwardMatrix() const;
    [[nodiscard]] Mat3x3 getColorMatrix() const;
    [[nodiscard]] Mat3x3 getAnalogBalanceMatrix() const;

    // HSV Maps
    void applyHSVMap(Color::HSV64 &val) const;
    void applyProfileLook(Color::HSV64 &val) const;
    [[nodiscard]] bool hasHSVMaps() const;

    // Factory methods
    static std::shared_ptr<CamProfile> MakeCamProfile(
        std::string_view model, double temp);
    static std::shared_ptr<CamProfile> MakeCamProfile(CamID id, double temp);

protected:
    // Seters
    void setCFAPattern(CFAPattern cfa);
    void setBlackLevel(Color::RGB64 black);
    void setWhiteLevel(Color::RGB64 white);
    void setBaseExposure(double baseExposure);
    void setActiveArea(Rect activeArea);
    void setCrop(Rect crop);
    void setForwardMatrix(const Mat3x3& fm);
    void setForwardMatrix(const Mat3x3& fm1, const Mat3x3& fm2);
    void setColorMatrix(const Mat3x3& cm1, const Mat3x3& cm2);
    void setHSVMaps(
        const HSVMap::HSV64Scale* data1, const HSVMap::HSV64Scale* data2);
    void setHSVMaps(int hueDim, int satDim, int valDim,
        const HSVMap::HSV64Scale* data1, const HSVMap::HSV64Scale* data2);
    void setProfileLook(const HSVMap::HSV64Scale* data);
    void setProfileLook(int hueDim, int satDim, int valDim,
        const HSVMap::HSV64Scale* data);
};

////////////////////////////////////////////////////////////////////////////////

inline CamProfile::CamProfile(double ctemp)
    : m_ctemp(ctemp)
{
}

inline CFAPattern CamProfile::getCFAPattern() const
{
    return m_cfa;
}

inline void CamProfile::setCFAPattern(CFAPattern cfa)
{
    assert(m_cfa.getFilter() != cfa.getFilter());
    m_cfa = cfa;
}

inline Color::RGB64 CamProfile::getBlackLevel() const
{
    return m_black;
}

inline void CamProfile::setBlackLevel(Color::RGB64 black)
{
    m_black = black;
}

inline Color::RGB64 CamProfile::getWhiteLevel() const
{
    return m_white;
}

inline void CamProfile::setWhiteLevel(Color::RGB64 white)
{
    m_white = white;
}

inline double CamProfile::getBaseExposure() const
{
    return m_baseExposure;
}

inline void CamProfile::setBaseExposure(double baseExposure)
{
    m_baseExposure = baseExposure;
}

inline Rect CamProfile::getActiveArea() const
{
    return m_activeArea;
}

inline void CamProfile::setActiveArea(Rect activeArea)
{
    m_activeArea = activeArea;
}

inline Rect CamProfile::getCrop() const
{
    return m_crop;
}

inline void CamProfile::setCrop(Rect crop)
{
    m_crop = crop;
}

inline double CamProfile::getIllu1() const
{
    return m_illu1;
}

inline double CamProfile::getIllu2() const
{
    return m_illu2;
}

inline Mat3x3 CamProfile::getForwardMatrix() const
{
    return m_forwardMat;
}

inline void CamProfile::setForwardMatrix(const Mat3x3& fm)
{
    m_forwardMat = fm;
}

inline void CamProfile::setForwardMatrix(const Mat3x3& fm1, const Mat3x3& fm2)
{
    m_forwardMat = Utils::interColorMat(
        m_ctemp, fm1, m_illu1, fm2, m_illu2);
}

inline Mat3x3 CamProfile::getColorMatrix() const
{
    return m_colorMat;
}

inline void CamProfile::setColorMatrix(const Mat3x3& cm1, const Mat3x3& cm2)
{
    CamProfile::m_colorMat = Utils::interColorMat(
        m_ctemp, cm1, m_illu1, cm2, m_illu2);
}

inline Mat3x3 CamProfile::getAnalogBalanceMatrix() const
{
    return m_analogBalance;
}

inline void CamProfile::setHSVMaps(
    const HSVMap::HSV64Scale* data1, const HSVMap::HSV64Scale* data2)
{
    // With default Adobe HSV dimensions
    constexpr int hueDim = 90, satDim = 30, valDim = 1;
    setHSVMaps(hueDim, satDim, valDim, data1, data2);
}

inline void CamProfile::setHSVMaps(int hueDim, int satDim, int valDim,
    const HSVMap::HSV64Scale* data1, const HSVMap::HSV64Scale* data2)
{
    m_hsvMap = std::make_shared<HSVMap>(
        hueDim, satDim, valDim, // Dimensions
        data1, m_illu1, data2, m_illu2, m_ctemp);
}

inline void CamProfile::setProfileLook(const HSVMap::HSV64Scale* data)
{
    // With default Adobe profile dimensions
    constexpr int hueDim = 36, satDim = 8, valDim = 16;
    setProfileLook(hueDim, satDim, valDim, data);
}

inline void CamProfile::setProfileLook(
    int hueDim, int satDim, int valDim, const HSVMap::HSV64Scale* data)
{
    m_profileLook = std::make_shared<HSVMap>(hueDim, satDim, valDim, data);
}
