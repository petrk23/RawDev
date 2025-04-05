## RawDev test CMake file with Google Test

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
## For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

## Omit gmock and installation
set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)

FetchContent_MakeAvailable(googletest)

## RawDev testing using Google Test
add_executable(RawDevTest
    Array2DTest.cpp
    ArtistNameValidatorTest.cpp
    CamProfileTest.cpp
    CFAPatternTest.cpp
    CmdLineTest.cpp
    ColorTest.cpp
    CR2ReaderTest.cpp
    Mat3x3Test.cpp
    OptionsTest.cpp
    PathTest.cpp
    pch.cpp
    pch.hpp
    RectTest.cpp
    StopWatchTest.cpp
    UtilsTest.cpp
    UtilsTestStat3.cpp
    WhiteBalanceTest.cpp
)
target_link_libraries(RawDevTest PRIVATE RawDevLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(RawDevTest)
