if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(VCPKG_TARGET_TRIPLET "custom-arm64-android" CACHE STRING "")
elseif(ANDROID_ABI STREQUAL "x86_64")
    set(VCPKG_TARGET_TRIPLET "custom-x64-android" CACHE STRING "")
endif()

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${ANDROID_NDK}/build/cmake/android.toolchain.cmake" CACHE FILEPATH "")
include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")