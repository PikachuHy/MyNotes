cmake_minimum_required(VERSION 3.5)

project(MyNotes LANGUAGES CXX
                VERSION "0.3.0"
        )
configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
option(USE_WEB_ENGINE_VIEW "use QWebEngineView" OFF)
option(ENABLE_TROJAN "enable trojan" OFF)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
if(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    if (APPLE)
        SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/MacOS)
        SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/PlugIns)
    endif()
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()
if(MSVC)
    add_compile_options("/utf-8")
    # fatal error C1189: #error:  WinSock.h has already been included
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()
# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Sql Concurrent Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Widgets LinguistTools Sql Concurrent
        Quick
        REQUIRED)
if (USE_WEB_ENGINE_VIEW)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS WebChannel Positioning REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
            WebEngineWidgets
            WebChannel
            Positioning
            REQUIRED)
else()

endif()
set(TS_FILES MyNotes_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        Widget.cpp
        Widget.h
        ListModel.cpp
        ListView.cpp
        TreeModel.cpp
        TreeItem.cpp
        TreeView.cpp
        DbModel.cpp
        DbManager.cpp
        Constant.cpp
        SearchDialog.cpp
        Utils.cpp
        ${TS_FILES}
        db.qrc
        css.qrc
        icon.qrc
        Toast.cpp
        ChooseFolderWidget.cpp
        Http.cpp
        Settings.cpp
        SettingsDialog.cpp
        PiWidget.cpp PiWidget.h
        LoginDialog.cpp LoginDialog.h
        AboutDialog.cpp AboutDialog.h
        SingleApplication.cpp SingleApplication.h
        FileSystemWatcher.cpp FileSystemWatcher.h
        ElasticSearchRestApi.cpp ElasticSearchRestApi.h
        PiDialog.cpp PiDialog.h
        LoginApi.cpp LoginApi.h
        TabWidget.cpp TabWidget.h
        TextPreview.cpp TextPreview.h
        TabBar.cpp TabBar.h
        )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
if(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
else()
if(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
endif()

if(ANDROID)
    add_library(MyNotes SHARED
        ${PROJECT_SOURCES}
    )
else()
    if (WIN32)
        set(app_WINRC icon.rc)
        set_property(SOURCE icon.rc APPEND PROPERTY
                OBJECT_DEPENDS ${PROJECT_SOURCE_DIR}/icon/icon.ico
                )
        add_executable(MyNotes WIN32
                ${PROJECT_SOURCES}
                ${app_WINRC}
                )
    else()
        add_executable(MyNotes
                ${PROJECT_SOURCES}
                )
    endif ()
endif()
add_subdirectory(CuteLogger)
add_subdirectory(QtMarkdownParser)
add_subdirectory(QtWordReader)
if(ENABLE_TROJAN)
    add_subdirectory(trojan)
    target_compile_definitions(MyNotes PRIVATE -DENABLE_TROJAN)
    target_link_libraries(MyNotes PRIVATE trojan)
    target_sources(MyNotes PRIVATE TrojanThread.cpp TrojanThread.h)
endif()
target_link_libraries(MyNotes PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Quick
        QtMarkdownParser
        QtMarkdownEditor
        QtWordReader
        CuteLogger
        )
if (USE_WEB_ENGINE_VIEW)
    target_compile_definitions(MyNotes PRIVATE -DUSE_WEB_ENGINE_VIEW)
    target_link_libraries(MyNotes PRIVATE
            Qt${QT_VERSION_MAJOR}::WebEngineWidgets
            Qt${QT_VERSION_MAJOR}::WebChannel
            Qt${QT_VERSION_MAJOR}::Positioning
            )
    target_sources(MyNotes PRIVATE WebEngineView.cpp WebEngineView.h)
else()
endif ()
