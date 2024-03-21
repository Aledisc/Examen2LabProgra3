#include "DuMessengerServer.h"
#include "DuMessengerSocket.h"
#include "QTextStream"
#include <QDebug>

namespace jdiscua
{
DuMessengerServer::DuMessengerServer(QObject *parent)
    : QTcpServer(parent)
{
}

bool DuMessengerServer::startServer(quint16 port)
{
    return listen(QHostAddress::Any, port);
}

void DuMessengerServer::incomingConnection(qintptr handle)
{
    qDebug() << "Se conecto el cliente con el handle: " << handle;
    auto socket = new DuMessengerSocket(handle, this);
    mSockets << socket;

    connect(socket, &DuMessengerSocket::DuReadyRead, [&](DuMessengerSocket *S)
    {
        qDebug() << "Se esta ejecutando ReadyRead";
        QTextStream T(S);
        auto text = T.readAll();

        for (auto i : mSockets){
            QTextStream K(i);
            K << text;
            i->flush();
        }
    });

    connect(socket, &DuMessengerSocket::DuStateChanged,
            [&](DuMessengerSocket *S, int ST) {
        qDebug() << "Se esta ejecutando el StateChanged con handle: " << S->socketDescriptor();
        if (ST == QTcpSocket::UnconnectedState){
            qDebug() << "- Estado desconectado con handle: "<< S->socketDescriptor();
            mSockets.removeOne(S);
            for(auto i : mSockets){
                QTextStream K(i);
                K << "Server: El cliente "
                  << S->socketDescriptor()
                  << "se ha desconectado...";
                i->flush();
            }
        }
    });
}
} // end namespace jdiscua





