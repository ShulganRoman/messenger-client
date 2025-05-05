cmake_minimum_required(VERSION 3.10)
project(messenger_client)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Boost REQUIRED COMPONENTS system thread json)

add_subdirectory(src/connection)
add_subdirectory(src/chat_window)
add_subdirectory(src/main_window)
add_subdirectory(src/auth)

add_executable(messenger_client src/main.cpp)

target_link_libraries(messenger_client
    PRIVATE
        Qt6::Core Qt6::Gui Qt6::Widgets
        Boost::system Boost::thread
        auth
        main_window
)
