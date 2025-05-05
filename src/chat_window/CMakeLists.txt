file(GLOB_RECURSE CHAT_WINDOW_LIB "*.cpp")
add_library(chat_window ${CHAT_WINDOW_LIB})

target_include_directories(chat_window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(chat_window PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Boost::system Boost::thread Boost::json connection)
