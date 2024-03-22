#include "dumessengerconnectiondialog.h"
#include "ui_dumessengerconnectiondialog.h"

namespace jdiscua
{
DuMessengerConnectionDialog::DuMessengerConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DuMessengerConnectionDialog)
{
    ui->setupUi(this);
}

DuMessengerConnectionDialog::~DuMessengerConnectionDialog()
{
    delete ui;
}

void DuMessengerConnectionDialog::on_accept_clicked()
{
    mHostName = ui->hostname->text();
    mPort = ui->port->value();
    accept();
}


void DuMessengerConnectionDialog::on_cancel_clicked()
{
    reject();
}


} // end namespace jdiscua

