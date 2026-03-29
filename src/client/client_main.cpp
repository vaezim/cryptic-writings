#include <iostream>

#include <QtWidgets/QApplication>

#include "main_window.h"
#include "startup_form.h"


int main(int argc, char **argv) {
    QApplication app(argc, argv);

    StartupForm startupForm;
    startupForm.show();

    MainWindow mainWindow;

    // Call mainWindow.show() once startupForm.close() has been called
    QObject::connect(
        &startupForm, &StartupForm::formClosed,
        &mainWindow, &MainWindow::show);

    // Emit ClientEndpointConfig from startupForm to mainWindow
    QObject::connect(
        &startupForm, &StartupForm::clientEndpointConfigReady,
        &mainWindow, &MainWindow::initializeClientEndpoint);

    app.exec();
}
