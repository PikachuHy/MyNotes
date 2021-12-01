cmake_minimum_required(VERSION 3.15)

project(MyNotes LANGUAGES CXX VERSION "0.6.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
####
#   Project Options
####
option(USE_WEB_ENGINE_VIEW "use QWebEngineView" OFF)
option(ENABLE_TROJAN "enable trojan" OFF)
option(UPDATE_TS "update translate" OFF)
option(BUILD_TEST "build test" OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
    message(STATUS "Configuring default CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
else ()
    message(STATUS "Configuring CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif ()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if (APPLE)
        set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/MacOS)
        set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/PlugIns)
    endif ()
endif ()
if (MSVC)
    add_compile_options("/utf-8")
    # fatal error C1189: #error:  WinSock.h has already been included
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    # CLion默认不会把依赖库加到环境变量中去，每次跑都是崩溃
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
endif ()


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

if (ANDROID)
    # 这个选项不起作用
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android")
    #    if (ANDROID_ABI STREQUAL "armeabi-v7a")
    #        set(ANDROID_EXTRA_LIBS
    #            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
    #            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
    #    endif()
endif ()
####
#   必须在根目录下添加Android的Target，否则json文件会出现问题
####
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Qml QuickControls2 WebView REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Qml QuickControls2 WebView REQUIRED)
option(USE_LOCAL_DEPS "use local dependency" ON)
if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MyNotes MANUAL_FINALIZATION)
else ()
    if (ANDROID)
        add_library(MyNotes SHARED "")
        find_package(QT NAMES Qt6 Qt5 COMPONENTS AndroidExtras REQUIRED)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AndroidExtras REQUIRED)
        target_link_libraries(MyNotes PUBLIC Qt${QT_VERSION_MAJOR}::AndroidExtras)
    else ()
        add_executable(MyNotes "")
    endif ()
endif ()
if (WIN32)
    set(app_WINRC icon.rc)
    set_property(SOURCE ${app_WINRC} APPEND PROPERTY
            OBJECT_DEPENDS src/assets/icon/icon.ico
            )
    target_sources(MyNotes PRIVATE
            src/assets/${app_WINRC}
            )
endif ()
target_sources(MyNotes PRIVATE main.cpp)
target_include_directories(MyNotes PRIVATE ${PROJECT_BINARY_DIR}/config src)
add_custom_target(genexdebug COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE_DIR:QtJieba>/$<TARGET_FILE_NAME:QtJieba>")
if (USE_LOCAL_DEPS)
    message(STATUS "include local")
    add_subdirectory(deps/QtMarkdown)
    add_subdirectory(deps/QtJieba)
    add_subdirectory(deps/QtWordReader)
    if (NOT ANDROID)
        add_subdirectory(deps/CuteLogger)
    endif ()
else ()
    include(cmake/DownloadDependencies.cmake)
endif ()

function(copy_dep dep_target_name)
    add_custom_command(
            TARGET MyNotes POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE_DIR:${dep_target_name}>/$<TARGET_FILE_NAME:${dep_target_name}>
            ${PROJECT_BINARY_DIR}/android-build/libs/${CMAKE_ANDROID_ARCH_ABI}/$<TARGET_FILE_NAME:${dep_target_name}>
            COMMENT "copy ${dep_target_name}"
    )
endfunction()
copy_dep(QtJieba)
copy_dep(QtWordReader)
copy_dep(QtMarkdownEditorCore)
copy_dep(QtQuickMarkdownEditor)
copy_dep(QtWidgetMarkdownEditor)
copy_dep(QtMarkdownRender)
copy_dep(QtMarkdownParser)
copy_dep(clatexmath)
add_subdirectory(src)
add_subdirectory(plugin)
if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(MyNotes)
endif ()
if (BUILD_TEST)
    add_subdirectory(test)
endif ()
