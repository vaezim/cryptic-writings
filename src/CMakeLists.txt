
include_directories(
    "common"
)

file(GLOB UI_FILES "ui/*.ui")
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

add_subdirectory(client)
add_subdirectory(server)
