file(GLOB_RECURSE MAINWINDOW_LIB "*.cpp")
add_library(main_window ${MAINWINDOW_LIB})

target_include_directories(main_window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(main_window PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Boost::system Boost::thread Boost::json chat_window)
