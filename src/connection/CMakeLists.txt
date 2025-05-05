file(GLOB_RECURSE CONNECTION_LIB "*.cpp")
add_library(connection ${CONNECTION_LIB})

target_include_directories(connection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(connection PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Boost::system Boost::thread Boost::json)
