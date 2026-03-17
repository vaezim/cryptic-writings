
set(CORE_SRC_FILES
    "chat_core.h"
    "chat_core.cpp"
    "key_gen.h"
    "key_gen.cpp"
)

set(GUI_SRC_FILES
    "main_window.h"
    "main_window.cpp"
)

add_executable("${PROJECT_BINARY_NAME}-cli"
    "client_cli.cpp"
    ${CORE_SRC_FILES}
)

add_executable("${PROJECT_BINARY_NAME}-gui"
    "client_gui.cpp"
    ${CORE_SRC_FILES}
    ${GUI_SRC_FILES}
)
