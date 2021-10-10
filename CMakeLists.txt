cmake_minimum_required(VERSION 3.15)

project(MyNotes LANGUAGES CXX
        VERSION "0.6.0"
        )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(USE_WEB_ENGINE_VIEW "use QWebEngineView" OFF)
option(ENABLE_TROJAN "enable trojan" OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
    message(STATUS "Configuring default CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Configuring CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if (APPLE)
        SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/MacOS)
        SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/PlugIns)
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

if(ANDROID)
    # 这个选项不起作用
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android")
    #    if (ANDROID_ABI STREQUAL "armeabi-v7a")
    #        set(ANDROID_EXTRA_LIBS
    #            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
    #            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
    #    endif()
endif()
####
#   必须在根目录下添加Android的Target，否则json文件会出现问题
####
option(BUILD_WITH_QT_QUICK "build with qt quick" ON)
if (BUILD_WITH_QT_QUICK)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Qml QuickControls2 REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Qml QuickControls2 REQUIRED)
    set(MyNotesQtQuick_TargetName MyNotesQtQuick)
    if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(${MyNotesQtQuick_TargetName} MANUAL_FINALIZATION)
    else ()
        if(ANDROID)
            # Qt5.15 默认用的是项目名.so，所以改一下名字
            set(MyNotesQtQuick_TargetName MyNotes)
            add_library(${MyNotesQtQuick_TargetName} SHARED "")
            find_package(QT NAMES Qt6 Qt5 COMPONENTS AndroidExtras REQUIRED)
            find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AndroidExtras REQUIRED)
            target_link_libraries(${MyNotesQtQuick_TargetName} PUBLIC Qt${QT_VERSION_MAJOR}::AndroidExtras)
        else()
            add_executable(${MyNotesQtQuick_TargetName} "")
        endif()
    endif ()
endif ()
add_subdirectory(src)
if (BUILD_WITH_QT_QUICK)
    qt_finalize_executable(${MyNotesQtQuick_TargetName})
endif ()