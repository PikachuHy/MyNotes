configure_file(config.h.in ${PROJECT_BINARY_DIR}/config/config.h)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_library(MyNotes_static "")
set_target_properties(MyNotes_static PROPERTIES AUTOMOC ON)
set_target_properties(MyNotes_static PROPERTIES AUTOUIC ON)
set_target_properties(MyNotes_static PROPERTIES AUTORCC ON)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(MyNotes_static PUBLIC -D_DEBUG)
endif ()

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
            REQUIRED
            )
endif ()
set(TS_FILES MyNotes_zh_CN.ts)
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
if (BUILD_WITH_QT_QUICK)
target_sources(${MyNotesQtQuick_TargetName} PRIVATE
        mobile/main.cpp
        mobile/KeyFilter.cpp mobile/KeyFilter.h
        mobile/FileSystem.cpp mobile/FileSystem.h
        mobile/Controller.cpp mobile/Controller.h
        mobile/qml/qml.qrc
        )

set_target_properties(${MyNotesQtQuick_TargetName} PROPERTIES AUTOMOC ON)
set_target_properties(${MyNotesQtQuick_TargetName} PROPERTIES AUTOUIC ON)
set_target_properties(${MyNotesQtQuick_TargetName} PROPERTIES AUTORCC ON)
target_include_directories(${MyNotesQtQuick_TargetName} PRIVATE . mobile)
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