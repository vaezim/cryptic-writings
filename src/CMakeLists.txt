
include_directories(
    "common"
)

if(NOT SERVER_ONLY)
    file(GLOB UI_FILES "ui/*.ui")
    set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")
    
    add_subdirectory(client)
endif()

add_subdirectory(server)
