#include "taskwindow.h"
#include "ui_taskwindow.h"
#include <QPointF>
TaskWindow::TaskWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TaskWindow)
{
    ui->setupUi(this);

}

TaskWindow::~TaskWindow()
{
    delete ui;
}

void TaskWindow::getflesh()
{
    json2.getAll();
    ui->Start_Box->clear();
    ui->End_Box->clear();
    for(int i = 0;i < json2.Site_vector.size();i++)
    {
        ui->Start_Box->addItem(json2.Site_vector[i].Site_Name);
        ui->End_Box->addItem(json2.Site_vector[i].Site_Name);
    }
}

void TaskWindow::on_Confirm_Button_clicked()
{


    int type = ui->TaskType_Box->currentIndex();   //任务类型   0-移库(目前正要用的) 1-出库 2-入库
    int priority = ui->Priority_Box->currentIndex();   //优先级 0-普通 1-优先
    QString start = ui->Start_Box->currentText();
    int startHigh = ui->StartHigh_Box->currentIndex();
    QString end = ui->End_Box->currentText();
    int endHigh = ui->EndHigh_Box->currentIndex();
    QString source = ui->lineEdit->text();
    QPointF startP = json2.Site_vector[ui->Start_Box->currentIndex()].start;
    QPointF endP   = json2.Site_vector[ui->End_Box->currentIndex()].start;
    emit SendTaskinfo(type,priority,start,startHigh,end,endHigh,source,startP,endP);
    this->close();
}

void TaskWindow::on_Cancel_Button_clicked()
{
    this->close();
}
