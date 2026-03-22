
set(CORE_SRC_FILES
    "client_endpoint.h"
    "client_endpoint.cpp"
    "key_gen.h"
    "key_gen.cpp"
)

set(GUI_SRC_FILES
    "main_window.h"
    "main_window.cpp"
    "startup_form.h"
    "startup_form.cpp"
)

add_executable("${PROJECT_BINARY_NAME}-client"
    "client_main.cpp"
    ${CORE_SRC_FILES}
    ${GUI_SRC_FILES}
    ${UI_FILES}
)

target_link_libraries("${PROJECT_BINARY_NAME}-client" PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
