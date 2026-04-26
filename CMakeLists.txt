cmake_minimum_required(VERSION 3.5)
project(cryptic-writings)

option(SERVER_ONLY "Compile server executable only" OFF)

set(PROJECT_BINARY_NAME "crywri")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CLIENT_EXECUTABLE_NAME "${PROJECT_BINARY_NAME}-client")
set(SERVER_EXECUTABLE_NAME "${PROJECT_BINARY_NAME}-server")

# Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(src)
