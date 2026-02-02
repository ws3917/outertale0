# 初始设置
cmake_minimum_required(VERSION 3.21)
project(speedtest)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(UNIX)
    set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
endif()

# 预加载第三方header-only库
include_directories("include")

# 需要使用的第三方库
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)

# 主项目：包含所有C++文件
file(GLOB_RECURSE SOURCES src/*.cpp)

if(WIN32)
    set(RC_FILES app/res.rc)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${RC_FILES})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3-shared
    SDL3_mixer::SDL3_mixer-shared
)

# 编译后配置
if(WIN32)
    # Release 模式下隐藏控制台窗口
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

# 复制资源文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_SOURCE_DIR}/assets/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)