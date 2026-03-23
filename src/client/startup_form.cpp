#include <QtWidgets/QStyle>
#include <QtGui/QIntValidator>
#include <QtGui/QRegularExpressionValidator>

#include "startup_form.h"


StartupForm::StartupForm() : QWidget() {
    m_ui = new Ui_StartupForm();
    m_ui->setupUi(this);

    // serverPortEdit accepts numbers between 0 and 65535
    QIntValidator *portValidator = new QIntValidator(0, 65535);
    m_ui->serverPortEdit->setValidator(portValidator);

    // serverAddressEdit accepts strings matching IP regex
    QString ipRegexPattern =
        R"(^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])$)";
    QRegularExpression re(ipRegexPattern);
    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(re);
    m_ui->serverAddressEdit->setValidator(ipValidator);

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
    m_ui->clientNameEdit->installEventFilter(this);
    m_ui->serverAddressEdit->installEventFilter(this);
    m_ui->serverPortEdit->installEventFilter(this);
    connect(m_ui->connectButton, &QPushButton::clicked,
        this, &StartupForm::on_connectButton_clicked);
}

void StartupForm::closeEvent(QCloseEvent *event) {
    emit formClosed();
    emit clientEndpointConfigReady(m_clientEndpointConfig);
    event->accept();
}

bool StartupForm::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() != QEvent::KeyPress) {
        return QObject::eventFilter(obj, event);
    }
    if (obj != m_ui->clientNameEdit &&
        obj != m_ui->serverAddressEdit &&
        obj != m_ui->serverPortEdit) {
        return QObject::eventFilter(obj, event);
    }
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
        on_connectButton_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void StartupForm::on_connectButton_clicked() {
    m_clientEndpointConfig.clientName = m_ui->clientNameEdit->text().toStdString();
    m_clientEndpointConfig.serverAddress = m_ui->serverAddressEdit->text().toStdString();
    m_clientEndpointConfig.serverPort = m_ui->serverPortEdit->text().toInt();
    close();
}
