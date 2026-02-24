cmake_minimum_required(VERSION 3.21)

set(VCPKG_OVERLAY_PORTS "${CMAKE_SOURCE_DIR}/vcpkg/ports")
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_SOURCE_DIR}/vcpkg/triplets")
project(yourBattle LANGUAGES C CXX)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -- 依赖管理 --
# vcpkg 仓库的库
find_package(SDL3 CONFIG REQUIRED)
find_package(yyjson CONFIG REQUIRED)
find_package(PhysFS CONFIG REQUIRED)
find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h")

# -- 项目构建 --
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

if(EMSCRIPTEN)
    add_executable(${PROJECT_NAME} ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "index"
        SUFFIX ".html"
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        "-sUSE_SDL=0" # 告诉 Emscripten 使用 vcpkg 提供的 SDL3，而不是自带的 SDL2 端口
        "-sALLOW_MEMORY_GROWTH=1"
        "-sEXIT_RUNTIME=1"
        "--shell-file" "${CMAKE_SOURCE_DIR}/app/index.html"
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        "--preload-file" "${CMAKE_SOURCE_DIR}/assets@/"
    )
elseif(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "main"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE android log)
elseif(WIN32)
    set(RC_FILES app/res.rc)
    add_executable(${PROJECT_NAME} ${SOURCES} ${RC_FILES})

    # 保持 Win7 兼容性
    if(MSVC)
        add_definitions(-D_WIN32_WINNT=0x0601 -DWINVER=0x0601)
    endif()

    # Release 模式下隐藏控制台窗口
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

if(EMSCRIPTEN)
    # Web 环境强制链接静态版本
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3-static
        PhysFS::PhysFS-static
        yyjson::yyjson
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3-shared
        PhysFS::PhysFS
        yyjson::yyjson
    )
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# -- 安装后配置 --
if(EMSCRIPTEN)
    file(COPY ${CMAKE_SOURCE_DIR}/app/icon.ico
        DESTINATION ${CMAKE_BINARY_DIR}/favicon.ico)
endif()

if(NOT ANDROID AND NOT EMSCRIPTEN)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E tar "c" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/main.pak" --format=zip "."
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/assets/
    )
    install(FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/main.pak DESTINATION .)
endif()
