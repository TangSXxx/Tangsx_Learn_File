#include "mainui.h"
#include "ui_mainui.h"
#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
MainUI::MainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainUI)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground); // 背景透明

    connect(this,SIGNAL(connecttoTcp(QString,QString,int)),&MainShow,SLOT(connectTcp(QString,QString,int)));
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::on_MainUI_Button_clicked()
{
    if(!MainShow.isVisible())
    {
    MainShow.show();
    }
    else
    {
      QMessageBox::warning(nullptr, "警告", "界面已经存在,请勿重复打开");
    }
}

void MainUI::on_Make_Button_clicked()
{
    int type = ui->AMRType_Box->currentIndex();
    QString ip = ui->AMRIP_Edit->text();
    QString name = ui->AMRName_Edit->text();
    emit connecttoTcp(ip,name,type);
}

void MainUI::on_TurnPage_Button_clicked()
{
    int index = ui->Mes_Widget->currentIndex();
    int all = ui->Mes_Widget->count();
    if(index+1 < all)
    {
        ui->Mes_Widget->setCurrentIndex(index+1);
    }
    else
    {
        ui->Mes_Widget->setCurrentIndex(0);
    }
}

void MainUI::on_Mine_Button_clicked()
{
    this->showMinimized();
}

void MainUI::on_Full_Button_clicked()
{
    if(!ui->Full_Button->isChecked())
    {
        this->showNormal();
    }
    else
    {
        this->showFullScreen();
    }
}
