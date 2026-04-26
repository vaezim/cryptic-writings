
set(CORE_SRC_FILES
    "server_endpoint.h"
    "server_endpoint.cpp"
)

add_executable(${SERVER_EXECUTABLE_NAME}
    "server_main.cpp"
    ${CORE_SRC_FILES}
)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Network
)

target_link_libraries(${SERVER_EXECUTABLE_NAME} PRIVATE
    Qt6::Core
    Qt6::Network
)
