cmake_minimum_required(VERSION 3.5)

project(MyNotes LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Sql Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Widgets LinguistTools Sql Concurrent
        WebEngineWidgets
        REQUIRED)

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
        ElasticSearchRestApi.cpp ElasticSearchRestApi.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MyNotes
        ${PROJECT_SOURCES}
    )
if(${CMAKE_BUILD_TYPE} EQUAL "MinSizeRel")
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
else()
    if(ANDROID)
        add_library(MyNotes SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(MyNotes
            ${PROJECT_SOURCES}
        )
    endif()

if(${CMAKE_BUILD_TYPE} EQUAL "MinSizeRel")
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
endif()
add_subdirectory(QtMarkdownParser)
target_link_libraries(MyNotes PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::WebEngineWidgets
        QtMarkdownParser
        )
