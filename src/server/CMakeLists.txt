
set(CORE_SRC_FILES
    "server_endpoint.h"
    "server_endpoint.cpp"
    "msg_switch.h"
    "msg_switch.cpp"
)

add_executable("${PROJECT_BINARY_NAME}-server"
    "server_main.cpp"
    ${CORE_SRC_FILES}
)

target_link_libraries("${PROJECT_BINARY_NAME}-server" PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
