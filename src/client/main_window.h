#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_main_window.h"


class MainWindow : public QMainWindow {
public:
    MainWindow() : QMainWindow() {}
    
    void setupUi();

private:
    Ui_MainWindow ui;
};

#endif // MAIN_WINDOW_H
