
set(CORE_SRC_FILES
    "client_endpoint.h"
    "client_endpoint.cpp"
)

set(GUI_SRC_FILES
    "main_window.h"
    "main_window.cpp"
    "startup_form.h"
    "startup_form.cpp"
)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
)

add_executable(${CLIENT_EXECUTABLE_NAME}
    "client_main.cpp"
    ${CORE_SRC_FILES}
    ${GUI_SRC_FILES}
    ${UI_FILES}
)

target_link_libraries(${CLIENT_EXECUTABLE_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    crypto
)
