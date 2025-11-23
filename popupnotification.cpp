#include "popupnotification.h"
#include "ui_popupnotification.h"
#include "mainwindow.h"
#include "mainui.h"
#include <QDebug>
popupNotification::popupNotification(QWidget *parent,QString ip) :
    QMainWindow(parent),
    ui(new Ui::popupNotification)
{
    ui->setupUi(this);

}

popupNotification::~popupNotification()
{
    delete ui;
}

void popupNotification::on_Confirm_Button_clicked()
{
    QString ip = ui->IPAddress_Edit->text();
    int type = ui->AMRType_Box->currentIndex();
    qDebug()<<"发送信号："<<ip<<type;
    emit SendConnect(ip,type);
    this->close();
}

void popupNotification::on_AMRType_Box_currentIndexChanged(int index)
{
    if(index == 2){
        ui->IPAddress_Edit->hide();
        ui->IPAddress_label->hide();
    }
}

void popupNotification::on_Cancel_Button_clicked()
{
    this->close();
}
