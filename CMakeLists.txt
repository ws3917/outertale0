cmake_minimum_required(VERSION 3.21)

set(VCPKG_OVERLAY_PORTS "${CMAKE_SOURCE_DIR}/vcpkg/ports")
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_SOURCE_DIR}/vcpkg/triplets")
project(spacetime0 LANGUAGES C CXX)

# C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -- 依赖管理 --
# vcpkg 仓库的库
find_package(PhysFS CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SFML COMPONENTS Graphics Window Audio System CONFIG REQUIRED)

# -- 项目构建 --
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

if(WIN32)
    set(RC_FILES app/res.rc)
    add_executable(${PROJECT_NAME} ${SOURCES} ${RC_FILES})

    # Release 模式下隐藏控制台窗口
    find_package(SFML COMPONENTS Main CONFIG REQUIRED)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# -- 链接 --
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MINIAUDIO_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics SFML::Window SFML::Audio SFML::System
    nlohmann_json::nlohmann_json
    PhysFS::PhysFS-static
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Main)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# -- 安装后配置 --
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar "c" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/main.pak" --format=zip "."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/assets/
)
install(FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/main.pak DESTINATION .)
