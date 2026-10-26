cmake_minimum_required(VERSION 3.16)
project(nova LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport WebEngineWidgets WebChannel)

add_executable(nova
    src/main.cpp
    src/main_window.cpp
    src/title_bar.cpp
    src/cosmic_background.cpp
    src/slide.cpp
    src/slide_deck.cpp
    src/slide_editor.cpp
    src/presentation_view.cpp
    src/toolbar.cpp
)

target_include_directories(nova PRIVATE src)

target_link_libraries(nova PRIVATE
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::WebEngineWidgets
    Qt6::WebChannel
)