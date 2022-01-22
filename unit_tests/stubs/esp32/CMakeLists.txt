add_library(stub_esp32 STATIC system_api.cpp cpu_start.cpp)
target_include_directories(stub_esp32 PUBLIC .)
add_library(stub::esp32 ALIAS stub_esp32)

#target_link_libraries(stub_esp32 stub::spi_flash)
