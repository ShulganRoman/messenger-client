file(GLOB_RECURSE AUTH_LIB "*.cpp")
add_library(auth ${AUTH_LIB})

target_include_directories(auth PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(auth PUBLIC connection Qt6::Core Qt6::Gui Qt6::Widgets Boost::system Boost::thread Boost::json)
