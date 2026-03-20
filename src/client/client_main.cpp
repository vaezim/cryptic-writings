#include <iostream>

#include <QtWidgets/QApplication>

#include "main_window.h"


int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setupUi();

    mainWindow.show();
    app.exec();
}
