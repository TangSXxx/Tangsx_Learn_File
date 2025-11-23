#include "minfoswindow.h"
#include "ui_minfoswindow.h"
#include <QDebug>
#include <thread>

MinFOSWindow::MinFOSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MinFOSWindow)
{
    ui->setupUi(this);

    for(int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        item->setTextAlignment(Qt::AlignCenter);
    } //ComListWidget
    for(int i = 0; i < ui->ComListWidget->count(); ++i) {
        QListWidgetItem *item = ui->ComListWidget->item(i);
        item->setTextAlignment(Qt::AlignCenter);
    }

}

MinFOSWindow::~MinFOSWindow()
{
    delete ui;
}

MinFOSWindow* MinFOSWindow::instance()
{
    static MinFOSWindow* fos = new MinFOSWindow();
    return fos;
}

void MinFOSWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    int ImageIndex = ui->listWidget->row(item);
    ui->listWidget->setFocus();

    if(ImageIndex >= ui->MainImageWidget->count()){
        qDebug()<<"MainImageWidget 索引超出限度！！！ "<<ImageIndex<<endl;
    }

    ui->MainImageWidget->setCurrentIndex(ImageIndex);
}


void MinFOSWindow::on_ComListWidget_itemClicked(QListWidgetItem *item)
{
    int ComListIndex =  ui->ComListWidget->row(item);
    ui->ComWidget->setCurrentIndex(ComListIndex);
}

void MinFOSWindow::on_StartTCPserver_Button_clicked()
{
    qDebug() << "启动TCP服务" << endl;
    FOSTcpServer* m = FOSTcpServer::instance();
    QString ip  = m->getLocalIP();
    quint16 port = 18087;
    qDebug() << "**ip**" << ip << "**port**" << port << endl;
    m->startListen(ip,port);
}
void MinFOSWindow::on_StopTCPserver_Button_clicked()
{
    FOSTcpServer* m = FOSTcpServer::instance();
    m->stopListen();

}

void MinFOSWindow::WriteMes(const QString& content){
    MinFOSWindow* fos  = instance();
    fos->ui->TCPServer_textEdit->append(content);
}


