#include <QtWidgets/QApplication>

#include "server_endpoint.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ServerEndpoint server;
    if (!server.initServer()) {
        return 1;
    }

    return app.exec();
}
