#include "main_window.h"


MainWindow::MainWindow() : QMainWindow() {
    m_ui = new Ui_MainWindow();
}

void MainWindow::setupUi() {
    m_ui->setupUi(this);

    connect(m_ui->sendButton, &QPushButton::clicked, this, &MainWindow::on_sendButton_clicked);

    m_ui->messageTextEdit->installEventFilter(this);
}

void MainWindow::on_sendButton_clicked() {
    QString message = m_ui->messageTextEdit->toPlainText();
    m_ui->messageTextEdit->clear();
    m_ui->textBrowser->append(message);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj != m_ui->messageTextEdit || event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(obj, event);
    }
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
        on_sendButton_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}
