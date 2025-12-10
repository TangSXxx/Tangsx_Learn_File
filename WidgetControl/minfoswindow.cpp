#include "minfoswindow.h"
#include "ui_minfoswindow.h"
#include <QDebug>
#include <thread>
#include "WidgetControl/tabwidgetcon.h"
#include "Function/http_server.h"
#include <QComboBox>

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

    // 4. 创建自定义控件，同样以标签页为父对象
    std::vector<std::string> Headers{"","地址","方式"};
    std::vector<int> headerstypes{0,2,1};
    std::vector<std::string> Items{"GET", "POST"};

    ui->tabGraph->setHeaders(Headers,headerstypes);
    ui->tabGraph->setqItems(Items);
    ui->tabGraph->initUI();

    connect(ui->addButton,&QPushButton::clicked,[this](){
        ui->tabGraph->addLine();
        int index = ui->tabGraph->GetColumnNum();
        tabMap[index - 1] = false;
    });

    connect(ui->deleteButton,&QPushButton::clicked,[this](){

        for(const auto& item : tabMap){
            if(item.second == true) ui->tabGraph->deleteLine(item.first);
        }

    });

    connect(ui->startButton,&QPushButton::clicked,[this](){
        //获取ip与端口信息
        QString ip = ui->HttpIp_edit1->text() + "." + ui->HttpIp_edit2->text() + "." + ui->HttpIp_edit3->text() + "." + ui->HttpIp_edit4->text();
        qint64 port = ui->HttpPort_edit->text().toLong();
        HTTP_SERVER* server = nullptr;
        if (ip == "..." || port <= 0 || port >= 65535) {
            qDebug() << "ip or port is wrong!";
            return;
        }
        else {
            server = HTTP_SERVER::instance(ip,port);
        }

        for(int row = 0; row < ui->tabGraph->GetColumnNum(); ++row ) {

           QString pathValue = "";
           QString methodValue = "";

           // 获取第二列的LineEdit数据
           QLineEdit* lineEdit = qobject_cast<QLineEdit*>(ui->tabGraph->GetCellWidget(row, 1));
           if (lineEdit) {
               pathValue = lineEdit->text().trimmed(); // 去除首尾空格
           }

           // 获取第三列的ComboBox数据
           QComboBox* comboBox = qobject_cast<QComboBox*>(ui->tabGraph->GetCellWidget(row, 2));
           if (comboBox) {
               methodValue = comboBox->currentText().trimmed();
           }

           // 打印或处理数据
           qDebug() << QString("第%1行: 输入框='%2', 下拉框='%3'")
                       .arg(row + 1)
                       .arg(pathValue)
                       .arg(methodValue);
           if(pathValue == "" || methodValue == "" ) continue;
           server->addRoute(pathValue, methodValue ,  [=](const QMap<QString, QString>& params) {
               return QString("\"result\":true , \"path\":%1 , \"method\":%2").arg(pathValue).arg(methodValue);
           });

        }

        // 启动服务器
        if (!server->startServer()) {
            qCritical() << "Failed to start server";
        }
    });

    connect(ui->tabGraph,&TabWidgetCon::TableStauChanged,[this](int row , int column ,bool status){
        qDebug() << "row" << row << "status" << status;
        tabMap[row] = true;
    });
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


