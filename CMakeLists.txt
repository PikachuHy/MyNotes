cmake_minimum_required(VERSION 3.5)

project(MyNotes LANGUAGES CXX
        VERSION "0.6.0"
        )
configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
option(USE_WEB_ENGINE_VIEW "use QWebEngineView" OFF)
option(ENABLE_TROJAN "enable trojan" OFF)
if (${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    if (APPLE)
        SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/MacOS)
        SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/MyNotes.app/Contents/PlugIns)
    endif ()
endif ()
# CLion默认不会把依赖库加到环境变量中去，每次跑都是崩溃
if (MSVC)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
endif ()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
add_library(libMyNotes "")
if (ANDROID)
else ()
    set_target_properties(libMyNotes PROPERTIES OUTPUT_NAME MyNotes)
endif ()
set_target_properties(libMyNotes PROPERTIES AUTOMOC ON)
set_target_properties(libMyNotes PROPERTIES AUTOUIC ON)
set_target_properties(libMyNotes PROPERTIES AUTORCC ON)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(libMyNotes PUBLIC -D_DEBUG)
endif ()
if (MSVC)
    add_compile_options("/utf-8")
    # fatal error C1189: #error:  WinSock.h has already been included
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()
if (ANDROID)
    message(STATUS "andorid abi ${CMAKE_ANDROID_ARCH_ABI}")
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/android-build/libs/${CMAKE_ANDROID_ARCH_ABI}/)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/android-build/libs/${CMAKE_ANDROID_ARCH_ABI}/)
endif ()
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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Sql Concurrent Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Widgets Sql Concurrent Network
        REQUIRED)
if (USE_WEB_ENGINE_VIEW)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS WebChannel Positioning REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
            WebEngineWidgets
            WebChannel
            Positioning
            REQUIRED)
else ()

endif ()
target_sources(libMyNotes PRIVATE MyNotes_zh_CN.ts)
target_sources(libMyNotes PRIVATE
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
        assets/db.qrc
        assets/css.qrc
        assets/icon.qrc
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
        Indexer.cpp Indexer.h
        )
option(UPDATE_TS "update translate" OFF)
if (UPDATE_TS)
    if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    else ()
        qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    endif ()
endif ()
option(BUILD_WITH_QT_QUICK "build with qt quick" ON)
if (BUILD_WITH_QT_QUICK)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick Qml REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Qml REQUIRED)
    qt_add_executable(MyNotesQtQuick
            MANUAL_FINALIZATION
            mobile/main.cpp
            mobile/KeyFilter.cpp mobile/KeyFilter.h
            mobile/qml/qml.qrc
            )
    qt_finalize_executable(MyNotesQtQuick)
    set_target_properties(MyNotesQtQuick PROPERTIES AUTOMOC ON)
    set_target_properties(MyNotesQtQuick PROPERTIES AUTOUIC ON)
    set_target_properties(MyNotesQtQuick PROPERTIES AUTORCC ON)
    target_link_libraries(MyNotesQtQuick PUBLIC libMyNotes
            QtQuickMarkdownItem
            Qt${QT_VERSION_MAJOR}::Quick
            Qt${QT_VERSION_MAJOR}::Qml
            )
endif ()
if (ANDROID)
else ()
    if (WIN32)
        set(app_WINRC icon.rc)
        set_property(SOURCE icon.rc APPEND PROPERTY
                OBJECT_DEPENDS ${PROJECT_SOURCE_DIR}/assets/icon/icon.ico
                )
        add_executable(MyNotes WIN32 desktop/main.cpp assets/${app_WINRC})
    else ()
        add_executable(MyNotes desktop/main.cpp)
    endif ()
    target_link_libraries(MyNotes PRIVATE libMyNotes)
endif ()
add_subdirectory(deps)
target_link_libraries(libMyNotes PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Network
        QtJieba
        QtMarkdownParser
        QtMarkdownEditor
        QtWordReader

        )
if (ANDROID)
else ()
    target_link_libraries(libMyNotes PUBLIC CuteLogger)
endif ()
if (USE_WEB_ENGINE_VIEW)
    target_compile_definitions(libMyNotes PRIVATE -DUSE_WEB_ENGINE_VIEW)
    target_link_libraries(libMyNotes PUBLIC
            Qt${QT_VERSION_MAJOR}::WebEngineWidgets
            Qt${QT_VERSION_MAJOR}::WebChannel
            Qt${QT_VERSION_MAJOR}::Positioning
            )
    target_sources(libMyNotes PRIVATE WebEngineView.cpp WebEngineView.h)
else ()
endif ()
if (BUILD_TEST)
    add_subdirectory(test)
endif ()
if (ANDROID)
    message(STATUS "andorid abi ${CMAKE_ANDROID_ARCH_ABI}")
    set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/android-build/libs/${CMAKE_ANDROID_ARCH_ABI}/)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/android-build/libs/${CMAKE_ANDROID_ARCH_ABI}/)
endif ()
