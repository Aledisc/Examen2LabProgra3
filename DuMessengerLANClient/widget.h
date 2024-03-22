#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

//QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
//QT_END_NAMESPACE
class QTcpSocket;

namespace jdiscua
{
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    int registrarse(QString username, QString password);
    int iniciarSesion(QString username, QString password);
    QString cuentaActual;
    int amigosConectados;
    int contarGenteConectada();
    void desconectarPersona();
    void enviarSolicitud(QString username);
    void chequearInbox();
    void aceptarAmigo();
    void eliminarSolicitud();


private slots:
    void on_send_clicked();
    void on_bind_clicked();
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked();

    void on_registrarseBtn_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_2_clicked();

    void on_entrarBoton_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket *mSocket;
};
} // end namespace jdiscua
#endif // WIDGET_H
