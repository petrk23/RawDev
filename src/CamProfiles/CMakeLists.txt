## RawDev camera profiles object library CMake file

add_library(CamProfilesLib OBJECT
    AllCamProfiles.hpp
    CamID.hpp
    CamProfile.cpp
    CamProfile.hpp
    CamProfileHelpers.hpp
    CFAPattern.cpp
    CFAPattern.hpp
    EOS_1DX/Cam1DX.cpp
    EOS_1DX/Cam1DX.hpp
    EOS_1DX/Cam1DX-hsv1.cpp
    EOS_1DX/Cam1DX-hsv2.cpp
    EOS_1DX/Cam1DX-prlook.cpp
    EOS_1DX2/Cam1DX2.cpp
    EOS_1DX2/Cam1DX2.hpp
    EOS_1DX2/Cam1DX2-hsv1.cpp
    EOS_1DX2/Cam1DX2-hsv2.cpp
    EOS_1DX2/Cam1DX2-prlook.cpp
    EOS_5D/Cam5D.cpp
    EOS_5D/Cam5D.hpp
    EOS_5D2/Cam5D2.cpp
    EOS_5D2/Cam5D2.hpp
    EOS_5D2/Cam5D2-prlook.cpp
    EOS_5D3/Cam5D3.cpp
    EOS_5D3/Cam5D3.hpp
    EOS_5D3/Cam5D3-hsv1.cpp
    EOS_5D3/Cam5D3-hsv2.cpp
    EOS_5D3/Cam5D3-prlook.cpp
    EOS_5D4/Cam5D4.cpp
    EOS_5D4/Cam5D4.hpp
    EOS_5D4/Cam5D4-hsv1.cpp
    EOS_5D4/Cam5D4-hsv2.cpp
    EOS_5D4/Cam5D4-prlook.cpp
    EOS_5Ds/Cam5Ds.cpp
    EOS_5Ds/Cam5Ds.hpp
    EOS_5Ds/Cam5Ds-hsv1.cpp
    EOS_5Ds/Cam5Ds-hsv2.cpp
    EOS_5Ds/Cam5Ds-prlook.cpp
    EOS_5DsR/Cam5DsR.cpp
    EOS_5DsR/Cam5DsR.hpp
    EOS_5DsR/Cam5DsR-hsv1.cpp
    EOS_5DsR/Cam5DsR-hsv2.cpp
    EOS_5DsR/Cam5DsR-prlook.cpp
    EOS_6D/Cam6D.cpp
    EOS_6D/Cam6D.hpp
    EOS_6D/Cam6D-hsv1.cpp
    EOS_6D/Cam6D-hsv2.cpp
    EOS_6D/Cam6D-prlook.cpp
    EOS_6D2/Cam6D2.cpp
    EOS_6D2/Cam6D2.hpp
    EOS_6D2/Cam6D2-hsv1.cpp
    EOS_6D2/Cam6D2-hsv2.cpp
    EOS_6D2/Cam6D2-prlook.cpp
    EOS_77D/Cam77D.cpp
    EOS_77D/Cam77D.hpp
    EOS_77D/Cam77D-hsv1.cpp
    EOS_77D/Cam77D-hsv2.cpp
    EOS_77D/Cam77D-prlook.cpp
    EOS_7D/Cam7D.cpp
    EOS_7D/Cam7D.hpp
    EOS_7D/Cam7D-prlook.cpp
    EOS_7D2/Cam7D2.cpp
    EOS_7D2/Cam7D2.hpp
    EOS_7D2/Cam7D2-hsv1.cpp
    EOS_7D2/Cam7D2-hsv2.cpp
    EOS_7D2/Cam7D2-prlook.cpp
    EOS_80D/Cam80D.cpp
    EOS_80D/Cam80D.hpp
    EOS_80D/Cam80D-hsv1.cpp
    EOS_80D/Cam80D-hsv2.cpp
    EOS_80D/Cam80D-prlook.cpp
)
