configure_file(config.h.in ${PROJECT_BINARY_DIR}/config/config.h)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(MyNotes_static "")
set_target_properties(MyNotes_static PROPERTIES AUTOMOC ON)
set_target_properties(MyNotes_static PROPERTIES AUTOUIC ON)
set_target_properties(MyNotes_static PROPERTIES AUTORCC ON)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(MyNotes_static PUBLIC -D_DEBUG)
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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Sql Concurrent Network QuickControls2 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Widgets Sql Concurrent Network QuickControls2
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
target_sources(MyNotes_static PRIVATE MyNotes_zh_CN.ts)
target_sources(MyNotes_static PRIVATE
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
        MainWindow.cpp MainWindow.h
        NoteController.cpp NoteController.h
        BeanFactory.cpp BeanFactory.h
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
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Qml QuickControls2 REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Qml QuickControls2 REQUIRED)
    set(MyNotesQtQuick_TargetName MyNotesQtQuick)
    if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(${MyNotesQtQuick_TargetName}
                MANUAL_FINALIZATION
                mobile/main.cpp
                mobile/KeyFilter.cpp mobile/KeyFilter.h
                mobile/FileSystem.cpp mobile/FileSystem.h
                mobile/Controller.cpp mobile/Controller.h
                mobile/qml/qml.qrc
                )
        qt_finalize_executable(${MyNotesQtQuick_TargetName})
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
        target_sources(${MyNotesQtQuick_TargetName} PRIVATE
                mobile/main.cpp
                mobile/KeyFilter.cpp mobile/KeyFilter.h
                mobile/FileSystem.cpp mobile/FileSystem.h
                mobile/Controller.cpp mobile/Controller.h
                mobile/qml/qml.qrc)
    endif ()
    set_target_properties(${MyNotesQtQuick_TargetName} PROPERTIES AUTOMOC ON)
    set_target_properties(${MyNotesQtQuick_TargetName} PROPERTIES AUTOUIC ON)
    set_target_properties(${MyNotesQtQuick_TargetName} PROPERTIES AUTORCC ON)
    target_link_libraries(${MyNotesQtQuick_TargetName} PUBLIC MyNotes_static
            QtQuickMarkdownItem
            Qt${QT_VERSION_MAJOR}::Quick
            Qt${QT_VERSION_MAJOR}::Qml
            PRIVATE
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::CorePrivate
            )
endif ()
if (ANDROID)
else ()
    if (WIN32)
        set(app_WINRC icon.rc)
        set_property(SOURCE icon.rc APPEND PROPERTY
                OBJECT_DEPENDS ${PROJECT_SOURCE_DIR}/assets/icon/icon.ico
                )
        add_executable(MyNotes WIN32
                assets/${app_WINRC}
                )
    else ()
        add_executable(MyNotes "")
    endif ()
    target_sources(MyNotes PRIVATE
            desktop/main.cpp
            desktop/qml/qml.qrc
            desktop/Clipboard.cpp desktop/Clipboard.h
            desktop/Controller.cpp desktop/Controller.h
            desktop/SettingDialogController.cpp desktop/SettingDialogController.h
            desktop/TreeModelAdaptor.cpp desktop/TreeModelAdaptor.h
            )
    target_link_libraries(MyNotes PRIVATE MyNotes_static
            Qt${QT_VERSION_MAJOR}::Quick
            Qt${QT_VERSION_MAJOR}::Qml
            QtQuickMarkdownItem
            )
    set_target_properties(MyNotes PROPERTIES AUTOMOC ON)
    set_target_properties(MyNotes PROPERTIES AUTORCC ON)
    target_include_directories(MyNotes PRIVATE ${PROJECT_BINARY_DIR}/config)
endif ()
####
#   add dependency
####
include(FetchContent)
message(STATUS "add dependency: QtMarkdown")
FetchContent_Declare(
        QtMarkdown
        GIT_REPOSITORY https://github.com/PikachuHy/QtMarkdown.git
        GIT_PROGRESS ON
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(QtMarkdown)
message(STATUS "add dependency: QtJieba")
FetchContent_Declare(
        QtJieba
        GIT_REPOSITORY https://github.com/PikachuHy/QtJieba.git
        GIT_TAG qt
        GIT_PROGRESS ON
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(QtJieba)
message(STATUS "add dependency: QtWordReader")
FetchContent_Declare(
        QtWordReader
        GIT_REPOSITORY https://github.com/PikachuHy/QtWordReader.git
        GIT_PROGRESS ON
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(QtWordReader)


target_link_libraries(MyNotes_static PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::QuickControls2
        QtJieba
        QtMarkdownParser
        QtMarkdownEditor
        QtWordReader
        )
if (ANDROID)
else ()
    message(STATUS "add dependency: CuteLogger")
    FetchContent_Declare(
            CuteLogger
            GIT_REPOSITORY https://github.com/PikachuHy/CuteLogger.git
            GIT_TAG qt6
            GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(CuteLogger)
    target_link_libraries(MyNotes_static PUBLIC CuteLogger)
endif ()
if (USE_WEB_ENGINE_VIEW)
    target_compile_definitions(MyNotes_static PRIVATE -DUSE_WEB_ENGINE_VIEW)
    target_link_libraries(MyNotes_static PUBLIC
            Qt${QT_VERSION_MAJOR}::WebEngineWidgets
            Qt${QT_VERSION_MAJOR}::WebChannel
            Qt${QT_VERSION_MAJOR}::Positioning
            )
    target_sources(MyNotes_static PRIVATE WebEngineView.cpp WebEngineView.h)
endif ()
if (BUILD_TEST)
    add_subdirectory(test)
endif ()