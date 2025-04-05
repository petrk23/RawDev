## Enable interprocedural optimizations (if possible)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${ipo_supported})
endif()
