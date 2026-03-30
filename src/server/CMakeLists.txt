
set(CORE_SRC_FILES
    "server_endpoint.h"
    "server_endpoint.cpp"
)

add_executable("${PROJECT_BINARY_NAME}-server"
    "server_main.cpp"
    ${CORE_SRC_FILES}
)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Network
)

target_link_libraries("${PROJECT_BINARY_NAME}-server" PRIVATE
    Qt6::Core
    Qt6::Network
)
