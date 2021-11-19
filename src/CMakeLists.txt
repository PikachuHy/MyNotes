configure_file(config.h.in ${PROJECT_BINARY_DIR}/config/config.h)

add_library(MyNotes_static "")
target_link_libraries(MyNotes PUBLIC MyNotes_static)
target_include_directories(MyNotes_static PUBLIC ${PROJECT_BINARY_DIR}/config)
target_include_directories(MyNotes_static PUBLIC .)

set_target_properties(MyNotes_static PROPERTIES AUTOMOC ON)
set_target_properties(MyNotes_static PROPERTIES AUTOUIC ON)
set_target_properties(MyNotes_static PROPERTIES AUTORCC ON)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(MyNotes_static PUBLIC -D_DEBUG)
endif ()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Sql Concurrent Network Qml Quick QuickControls2 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql Concurrent Network Qml Quick QuickControls2 REQUIRED)

set(TS_FILES MyNotes_zh_CN.ts)
file(GLOB_RECURSE DIR_SRCS *.h *.cpp *.qrc)
target_sources(MyNotes_static PRIVATE ${DIR_SRCS})
if (UPDATE_TS)
    if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    else ()
        qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    endif ()
endif ()

target_link_libraries(MyNotes_static PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::CorePrivate
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::QuickControls2
        Qt${QT_VERSION_MAJOR}::WebView
        QtJieba
        QtWidgetMarkdownEditor
        QtQuickMarkdownEditor
        QtMarkdownParser
        QtWordReader
        )
if (NOT ANDROID)
    target_link_libraries(MyNotes_static PUBLIC CuteLogger)
endif ()

