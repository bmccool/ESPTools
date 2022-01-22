cmake_minimum_required(VERSION 3.5)

project(ESP-OLED_unit_tests LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stubs/esp32)
add_subdirectory(lib/Catch2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../main/unit_tests maintests)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stubs/esp32)
add_executable(unit_tests)
target_link_libraries(unit_tests
    Catch2::Catch2WithMain
    3d_stuff_tests
)


