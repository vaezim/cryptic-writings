#include <QtWidgets/QStyle>

#include "startup_form.h"


StartupForm::StartupForm() : QWidget() {
    m_ui = new Ui_StartupForm();
    m_ui->setupUi(this);

    // Show window in the center of the screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
}

void StartupForm::closeEvent(QCloseEvent *event) {
    emit formClosed();
    event->accept();
}
