# 初始设置
cmake_minimum_required(VERSION 3.21)
project(psot_sans_battle)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(PLATFORM_DIR "msvc")
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(UNIX)
    set(PLATFORM_DIR "gcc")
    set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
endif()

# 预加载第三方库
file(GLOB THIRD_PARTY_DIRS "3rdparty/${PLATFORM_DIR}/*")

if(THIRD_PARTY_DIRS)
    list(PREPEND CMAKE_PREFIX_PATH ${THIRD_PARTY_DIRS})
endif()

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

    # 复制DLL
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()

# 复制资源文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_SOURCE_DIR}/assets/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)

# 安装
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
    DESTINATION .
)

if(WIN32)
    install(IMPORTED_RUNTIME_ARTIFACTS
        SDL3::SDL3-shared
        SDL3_mixer::SDL3_mixer-shared
        RUNTIME DESTINATION .
    )
endif()