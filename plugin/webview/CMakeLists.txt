find_package(QT NAMES Qt6 Qt5 COMPONENTS WebChannel Positioning REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS WebEngineWidgets WebChannel Positioning REQUIRED)
add_library(WebViewPlugin WebEngineView.cpp WebEngineView.h)
set_target_properties(WebViewPlugin PROPERTIES AUTOMOC ON)
set_target_properties(WebViewPlugin PROPERTIES AUTOUIC ON)
set_target_properties(WebViewPlugin PROPERTIES AUTORCC ON)
target_compile_definitions(WebViewPlugin PUBLIC -DUSE_WEB_ENGINE_VIEW)
target_link_libraries(WebViewPlugin PUBLIC
        Qt${QT_VERSION_MAJOR}::WebEngineWidgets
        Qt${QT_VERSION_MAJOR}::WebChannel
        Qt${QT_VERSION_MAJOR}::Positioning
        )
target_link_libraries(MyNotes PUBLIC WebViewPlugin)