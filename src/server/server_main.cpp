#include <QtCore/QCoreApplication>

#include "server_endpoint.h"


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    ServerEndpoint server;
    if (!server.initServer()) {
        return 1;
    }

    return app.exec();
}
