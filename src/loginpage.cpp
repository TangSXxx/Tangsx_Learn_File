#include "loginpage.h"
#include "ui_loginpage.h"
#include <QMouseEvent>
#include <QMessageBox>
#include "minfoswindow.h"
LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
     //setAttribute(Qt::WA_TranslucentBackground);setAttribute(Qt::WA_TranslucentBackground);  //窗口透明

    connect(ui->Button_close,&QPushButton::clicked, this,[=]()  // 点击关闭按钮
    {
        this->close();
    });
    connect(ui->Regis_Button,&QPushButton::clicked, this,[=]()  // 点击注册按钮
    {
        // 信息提示框
        QMessageBox::information(this, "标题", "这是一个信息提示框。");
    });

}

void LoginPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bPressed = true;
        m_point = event->pos(); // 记录按下时鼠标在窗口内的位置
    }
    // 可选择性调用基类事件处理，确保其他鼠标操作正常
    // QMainWindow::mousePressEvent(event);
}

void LoginPage::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bPressed)
    {
        // 计算窗口移动后的新位置
        move(event->globalPos() - m_point);
    }
}

void LoginPage::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bPressed = false;
    }
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::on_Login_Button_clicked()
{
   QString user_str  = ui->lineEdit_username->text();
   QString password = ui->lineEdit_password->text();
   if(user_str == "root" && password == "031819"){
       this->close();
       MinFOSWindow* mainProgram  = MinFOSWindow::instance();
       mainProgram->show();
   }else{
       // 错误提示框
       QMessageBox::critical(this, "错误", "账号或密码输入有误，请重新输入！");
   }
}
