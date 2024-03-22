#include "widget.h"
#include "ui_widget.h"
#include "dumessengerconnectiondialog.h"
#include <QTcpSocket>
#include <QTextStream>
#include "QFile"
#include "QTimer"

namespace jdiscua
{
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    mSocket = new QTcpSocket(this);

    connect(mSocket, &QTcpSocket::readyRead, [&](){
        QTextStream T(mSocket);
        auto text = T.readAll();
        ui->textEdit->append(text);
    });
    amigosConectados=0;
    ui->textEdit->setReadOnly(true);
    /*
    QFile usersEnLinea("C:/Users/Admin/Desktop/archivosWhatsapp/usersEnLinea.txt");
    if(!usersEnLinea.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)){
        qDebug() << "Ocurrio un problema al crear el archivo de gente conectada";
    }
*/

}


Widget::~Widget()
{
    delete ui;
}

int Widget::registrarse(QString username, QString password)
{
    QFile file("C:/Users/Admin/Desktop/archivosWhatsapp/usersWhatsapp.txt");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)) {
        qDebug() << "Error al abrir el archivo.";
        return -1;
    }

    QTextStream out(&file);
    out.seek(file.size()); // Coloca el cursor al final del archivo
    if (file.size() != 0) {
        out << "\n"; // Agrega un salto de línea si el archivo no está vacío
    }
    out << username << "," << password << ",0"; // Agrega el nuevo usuario al archivo

    file.close();

    QFile friendsFile("C:/Users/Admin/Desktop/archivosWhatsapp/" + username + "_friends.txt");
    if (!friendsFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error al crear el archivo de amigos.";
        return -1;
    }

    QFile inbox("C:/Users/Admin/Desktop/archivosWhatsapp/" + username + "_inbox.txt");
    if(!inbox.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error al crear el archivo de inbox,";
        return -1;
    }

    friendsFile.close();

    return 1;
}

int Widget::iniciarSesion(QString username, QString password)
{
    QFile file("C:/Users/Admin/Desktop/archivosWhatsapp/usersWhatsapp.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir el archivo.";
        return -1;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() >= 2 && fields[0] == username && fields[1] == password) {
            file.close();
            return 1; // Usuario y contraseña coinciden
        }
    }


    file.close();
    return 0; // Usuario no encontrado o contraseña incorrecta
}

int Widget::contarGenteConectada()
{
    QFile archivo("C:/Users/Admin/Desktop/archivosWhatsapp/personasConectadas.txt");
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir el archivo";
        return -1;
    }

    QTextStream in(&archivo);
    int contar = in.readLine().toInt();
    archivo.close();

    return contar;
}

void Widget::desconectarPersona()
{
    QString filePath = "C:/Users/Admin/Desktop/archivosWhatsapp/personasConectadas.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir el archivo";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    file.close();

    bool ok;
    int personasConectadas = line.toInt(&ok);
    if (!ok) {
        qDebug() << "Error al convertir el número de personas conectadas";
        return;
    }

    personasConectadas--;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Error al abrir el archivo para escribir";
        return;
    }

    QTextStream out(&file);
    out << QString::number(personasConectadas) << "\n";
    file.close();
}

void Widget::enviarSolicitud(QString username)
{
    QFile file("C:/Users/Admin/Desktop/archivosWhatsapp/usersWhatsapp.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir el archivo de usuarios";
        return;
    }

    QTextStream in(&file);
    bool userFound = false;
    QString userData;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList userDataList = line.split(",");
        if (userDataList.size() >= 3 && userDataList[0] == username) {
            userFound = true;
            userData = line;
            break;
        }
    }

    file.close();

    if (!userFound) {
        qDebug() << "Usuario no encontrado";
        return;
    }

    // Obtener nombre y crear solicitud
    QStringList userDataList = userData.split(",");
    QString nombre = userDataList[2];
    QString solicitud = QString("te ha enviado una solicitud de amistad: %1 ").arg(cuentaActual);

    // Agregar solicitud al archivo de inbox del destinatario
    QString fileName = QString("%1_inbox.txt").arg(username);
    QFile inboxFile("C:/Users/Admin/Desktop/archivosWhatsapp/" + username + "_inbox.txt");
    if (!inboxFile.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Error al abrir el archivo de inbox del destinatario";
        return;
    }

    QTextStream out(&inboxFile);
    out << solicitud << "\n";
    inboxFile.close();

    qDebug() << "Solicitud enviada a" << nombre;
}

void Widget::chequearInbox()
{

    QFile file("C:/Users/Admin/Desktop/archivosWhatsapp/" + cuentaActual + "_inbox.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir el archivo de inbox";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        // Agregar la línea como un item al QListWidget
        ui->inbox->addItem(line);
    }

    file.close();
}

void Widget::aceptarAmigo()
{
    QListWidgetItem *item = ui->inbox->currentItem();
    if (!item) return;

    QString mensaje = item->text();
    QStringList partes = mensaje.split(": ");
    if (partes.size() != 2) return;

    QString usuario = partes[1];
    QFile file("C:/Users/Admin/Desktop/archivosWhatsApp/" + cuentaActual + "_inbox.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QStringList lineas;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString linea = in.readLine();
        if (!linea.contains(usuario)) {
            lineas.append(linea);
        }
    }
    file.close();

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &linea : lineas) {
            out << linea << "\n";
        }
        file.close();
    }

    delete ui->inbox->takeItem(ui->inbox->currentRow());

    QFile fileAmigos("C:/Users/Admin/Desktop/archivosWhatsApp/" + cuentaActual + "_friends.txt");
    if (fileAmigos.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&fileAmigos);
        out << usuario << "\n";
        fileAmigos.close();
    }

}

void Widget::eliminarSolicitud()
{
    QListWidgetItem *item = ui->inbox->currentItem();
    if (!item) return;

    QString mensaje = item->text();
    QStringList partes = mensaje.split(": ");
    if (partes.size() != 2) return;

    QString usuario = partes[1];
    QFile file("C:/Users/Admin/Desktop/WhatsApp/" + cuentaActual + "_inbox.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QStringList lineas;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString linea = in.readLine();
        if (!linea.contains(usuario)) {
            lineas.append(linea);
        }
    }
    file.close();

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &linea : lineas) {
            out << linea << "\n";
        }
        file.close();
    }

    QListWidgetItem *itemToRemove = ui->inbox->takeItem(ui->inbox->currentRow());
    delete itemToRemove;
}










void Widget::on_send_clicked()
{
    QTextStream T(mSocket);
    T << ui->nickname->text() << ": " << ui->message->text();
    mSocket->flush();
    ui->message->clear();
}


void Widget::on_bind_clicked()
{
    DuMessengerConnectionDialog D(this);
    if (D.exec() == QDialog::Rejected){
        return;
    }
    mSocket->connectToHost(D.hostname(), D.port());
    ui->stackedWidget->setCurrentIndex(1);
    ui->entrarBoton->setVisible(true);
    QString qs = "Personas conectadas: " + QString::number(contarGenteConectada());
    ui->personasOnline->setText(qs);
    /*
    QFile usersEnLinea("C:/Users/Admin/Desktop/archivosWhatsapp/usersEnLinea.txt");
    if(!usersEnLinea.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)){
        qDebug() << "Ocurrio un problema al crear el archivo de gente conectada";
    }
    QTextStream qts(&usersEnLinea);
    qts << cuentaActual;
    usersEnLinea.close();
    */
}


void Widget::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->entrarBoton->setVisible(false);
}


void Widget::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void Widget::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void Widget::on_pushButton_3_clicked()
{
    QString username = ui->nickname->text();
    QString password = ui->password->text();
    if(iniciarSesion(username, password)==1){
        cuentaActual=username;
        ui->stackedWidget->setCurrentIndex(4);
        ui->labelUser->setText(cuentaActual);
        ui->userRegistrarse->setText("");
        ui->passRegistrarse->setText("");
        QString qs = "Personas conectadas: " + QString::number(contarGenteConectada());
        ui->personasOnline->setText(qs);
    }else{
        QString alerta = "Inicio de sesion fallido";
        ui->alertaIniciarSesion->setText(alerta);
    }

}


void Widget::on_registrarseBtn_clicked()
{
    QString username = ui->userRegistrarse->text();
    QString password = ui->passRegistrarse->text();
    if(registrarse(username,password)==1){
        //cuentaActual=username;
        ui->stackedWidget->setCurrentIndex(0);
        //ui->labelUser->setText(cuentaActual);
        ui->userRegistrarse->setText("");
        ui->passRegistrarse->setText("");
        QString qs = "Personas conectadas: " + QString::number(contarGenteConectada());
        ui->personasOnline->setText(qs);
    }else{
        qDebug() << "Registro fallido";

    }
}


void Widget::on_pushButton_8_clicked()
{
    /*
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);
    ui->nickname->setText("");
    ui->password->setText("");
    */
    desconectarPersona();
    exit(0);
}


void Widget::on_pushButton_13_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    QString qs = "Personas conectadas: " + QString::number(contarGenteConectada());
    ui->personasOnline->setText(qs);
}


void Widget::on_pushButton_5_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}


void Widget::on_pushButton_7_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}


void Widget::on_pushButton_10_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
    chequearInbox();
}


void Widget::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void Widget::on_entrarBoton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void Widget::on_pushButton_9_clicked()
{
    QString username = ui->amigoUser->text();
    enviarSolicitud(username);
    ui->alertaAmigo->setText("Se ha enviado la solicitud");
    qDebug() << "Esperando 2 segundos...";
    QTimer::singleShot(2000, [&](){
        qDebug() << "Han pasado 2 segundos.";
        ui->alertaAmigo->setText("");
    });
}


void Widget::on_pushButton_11_clicked()
{
    aceptarAmigo();
    chequearInbox();
}


void Widget::on_pushButton_12_clicked()
{
    eliminarSolicitud();
    chequearInbox();
}


} // end namespace jdiscua

