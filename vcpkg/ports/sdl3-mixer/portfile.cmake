vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO libsdl-org/SDL_mixer
    REF "prerelease-${VERSION}"
    SHA512 3059ccf7d69861cec325a7d52fcfae7ac7760d4d97236ee4252d0587c0638df8e1d4a22d537e56438fc48668a4ac368c1db3599493821a5267d98d22e7449c18
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
    ${FEATURE_OPTIONS}
    -DSDLMIXER_VENDORED=OFF
)
vcpkg_cmake_install()
vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()

if(EXISTS "${CURRENT_PACKAGES_DIR}/cmake")
    vcpkg_cmake_config_fixup(PACKAGE_NAME SDL3_mixer CONFIG_PATH cmake)
else()
    vcpkg_cmake_config_fixup(PACKAGE_NAME SDL3_mixer CONFIG_PATH lib/cmake/SDL3_mixer)
endif()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/share"
    "${CURRENT_PACKAGES_DIR}/debug/include"
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.txt")