#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_main_window.h"


class MainWindow : public QMainWindow {
public:
    MainWindow();
    
    void setupUi();

private:
    Ui_MainWindow *m_ui;

private slots:
    void on_sendButton_clicked();
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // MAIN_WINDOW_H
