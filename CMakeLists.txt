cmake_minimum_required(VERSION 3.31)
project(RaspberryPiGpioInteraction CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/local/include)

find_library(GPIOD_CXX_LIB gpiodcxx REQUIRED)

add_executable(RaspberryPiGpioInteraction main.cpp
#        ledblinkenlassen.cpp
#        auftastermitledreagieren.cpp
#        auftasterreagieren.cpp
)

target_link_libraries(RaspberryPiGpioInteraction PRIVATE ${GPIOD_CXX_LIB})