cmake_minimum_required(VERSION 3.5)
project(cryptic-writings)

set(PROJECT_BINARY_NAME "crywri")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#----------------------------------------
# Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
)
#----------------------------------------

add_subdirectory(src)
