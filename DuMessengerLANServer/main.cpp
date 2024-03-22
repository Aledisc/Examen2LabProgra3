#include <QCoreApplication>
#include "QDebug"
#include "DuMessengerServer.h"

using namespace jdiscua;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "al menos corre";
    DuMessengerServer Server;
    if (!Server.startServer(3333)) {
        qDebug() << "Error: " << Server.errorString();
        return 1;
    }
    qDebug() << "Servidor iniciado...";
    return a.exec();
}
