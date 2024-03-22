#include "DuMessengerServer.h"
#include "DuMessengerSocket.h"
#include "QTextStream"
#include <QDebug>
#include "QFile"

namespace jdiscua
{
DuMessengerServer::DuMessengerServer(QObject *parent)
    : QTcpServer(parent)
{
}

bool DuMessengerServer::startServer(quint16 port)
{
    qDebug() << "prendiendo server";
    personasConectadas=0;
    return listen(QHostAddress::Any, port);
}

void DuMessengerServer::incomingConnection(qintptr handle)
{
    qDebug() << "Se conecto el cliente con el handle: " << handle;
    auto socket = new DuMessengerSocket(handle, this);
    mSockets << socket;
    personasConectadas++;

    QFile personasArchivo("C:/Users/Admin/Desktop/archivosWhatsapp/personasConectadas.txt");
    if (!personasArchivo.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error al abrir el archivo de personas.";
        //return;
    }
    QTextStream stream(&personasArchivo);
    stream << personasConectadas;
    personasArchivo.close();

    for (auto i : mSockets){
        QTextStream T(i);
        T << "Server: Se ha conectado: " << handle;
        i->flush();
    }

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
                K << "Server: Un cliente "
                  //<< S->socketDescriptor()
                  << "se ha desconectado...";
                personasConectadas--;
                i->flush();
            }
            QFile personasArchivo("C:/Users/Admin/Desktop/archivosWhatsapp/personasConectadas.txt");
            if (!personasArchivo.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                qDebug() << "Error al abrir el archivo de personas.";
                //return;
            }
            QTextStream stream(&personasArchivo);
            stream << personasConectadas;
            personasArchivo.close();

        }
    });
}
} // end namespace jdiscua





