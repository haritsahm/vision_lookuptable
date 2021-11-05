cmake_minimum_required(VERSION 3.0.2)
project(vision_lookuptable)
add_compile_options(-std=c++11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp"
    image_clip.cpp
    main_window.cpp
    gui.ui
    )

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets)

