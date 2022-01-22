#include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#add_library(3d_stuff STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../3d_stuff.cpp)
#idf_component_register(SRCS "3d_stuff.test.cpp"
#                    INCLUDE_DIRS ""
#                    COMPONENT_REQUIRES esp_system)
# This would be if I actually separated out c files from headers like I'm supposed to

#Instead, since I'm using just a header file, I have to do
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(3d_stuff_tests OBJECT 3d_stuff.test.cpp)
target_link_libraries(3d_stuff_tests PRIVATE Catch2::Catch2)
#target_link_libraries(3d_stuff_tests PRIVATE 3d_stuff)
