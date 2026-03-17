
set(CORE_SRC_FILES
    "client_handler.h"
    "client_handler.cpp"
    "msg_switch.h"
    "msg_switch.cpp"
)

add_executable("${PROJECT_BINARY_NAME}-server"
    "server.cpp"
    ${CORE_SRC_FILES}
)
