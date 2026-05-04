#include <QtWidgets/QStyle>

#include "main_window.h"


static const char *placeHolderText = R"(
=========================================================================
 _____                  _   _        _    _      _ _   _
/  __ \                | | (_)      | |  | |    (_) | (_)
| /  \/_ __ _   _ _ __ | |_ _  ___  | |  | |_ __ _| |_ _ _ __   __ _ ___
| |   | '__| | | | '_ \| __| |/ __| | |/\| | '__| | __| | '_ \ / _` / __|
| \__/\ |  | |_| | |_) | |_| | (__  \  /\  / |  | | |_| | | | | (_| \__ \
 \____/_|   \__, | .__/ \__|_|\___|  \/  \/|_|  |_|\__|_|_| |_|\__, |___/
             __/ | |                                            __/ |
            |___/|_|                                           |___/
=========================================================================
                      End-to-end encrypted chat ;)

)";

MainWindow::MainWindow() : QMainWindow() {
    m_ui = new Ui_MainWindow();
    m_ui->setupUi(this);

    // Welcome message
    QFont font("Courier New");
    font.setStyleHint(QFont::Monospace);
    font.setBold(true);
    m_ui->textBrowser->setFont(font);
    m_ui->textBrowser->setLineWrapMode(QTextEdit::NoWrap);
    m_ui->textBrowser->append(placeHolderText);

    // Show window in the center of the screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );

    // Handle Enter-key presses
    m_ui->messageTextEdit->installEventFilter(this);
    connect(m_ui->sendButton, &QPushButton::clicked,
        this, &MainWindow::onSendButtonClicked);
}

void MainWindow::initializeClientEndpoint(const ClientEndpointConfig &clientEndpointConfig) {
    m_clientEndpointPtr = std::make_unique<ClientEndpoint>(clientEndpointConfig);
    QString text = QString("::::: User [%1] joined the server :::::\n")
        .arg(m_clientEndpointPtr->clientName().c_str());
    m_ui->textBrowser->append(text);

    // When client receives a new message, display it
    connect(m_clientEndpointPtr.get(), &ClientEndpoint::receivedMessage,
        this, &MainWindow::onNewMessageArrive);
}

void MainWindow::onSendButtonClicked() {
    QString message = m_ui->messageTextEdit->toPlainText();
    m_ui->messageTextEdit->clear();
    if (message.isEmpty()) {
        return;
    }

    // Append it to textBrowser
    QString clientName = QString("[%1] ")
        .arg(m_clientEndpointPtr->clientName().c_str());
    message.prepend(clientName);
    m_ui->textBrowser->append(message);
    m_ui->textBrowser->moveCursor(QTextCursor::End);

    // Send message to the server
    if (m_clientEndpointPtr) {
        m_clientEndpointPtr->sendMessage(message);
    }
}

void MainWindow::onNewMessageArrive(const QString &message) {
    m_ui->textBrowser->append(message);
    m_ui->textBrowser->moveCursor(QTextCursor::End);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj != m_ui->messageTextEdit || event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(obj, event);
    }
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
        onSendButtonClicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}
