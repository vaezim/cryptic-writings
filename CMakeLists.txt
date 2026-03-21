cmake_minimum_required(VERSION 3.5)
project(cryptic-writings)

option(SERVER_ONLY "Compile server executable only" OFF)

set(PROJECT_BINARY_NAME "crywri")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(SERVER_ONLY)
    message(WARNING "Building server executable only...")
else()
    #----------------------------------------
    # Qt (dependency for client executable)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    find_package(Qt6 REQUIRED COMPONENTS
        Core
        Widgets
        Network
    )
    #----------------------------------------
endif()

add_subdirectory(src)
