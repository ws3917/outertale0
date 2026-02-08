cmake_minimum_required(VERSION 3.21)

# 设置平台特有选项
if(WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
    file(GLOB THIRD_PARTY_LIBS 3rdparty/*)

    foreach(LIB IN LISTS THIRD_PARTY_LIBS)
        list(APPEND CMAKE_PREFIX_PATH ${LIB})
    endforeach()
endif()

project(outertale0)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 预加载第三方header-only库
include_directories("include")

# 需要使用的第三方库
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)

# 主项目：包含所有C++文件
file(GLOB_RECURSE SOURCES src/*.cpp)

if(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "main"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE android log)
elseif(WIN32)
    set(RC_FILES app/res.rc)
    add_executable(${PROJECT_NAME} ${SOURCES} ${RC_FILES})
    # Release 模式下隐藏控制台窗口
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3-shared
    SDL3_mixer::SDL3_mixer-shared
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 编译后配置
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
    install(FILES
        $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        DESTINATION .
    )
endif()

if(NOT ANDROID)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_SOURCE_DIR}/assets/
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/
        DESTINATION .
    )
endif()
