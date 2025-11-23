#ifndef MAINUI_H
#define MAINUI_H
#include <QMouseEvent>
#include <QWidget>
#include <QMainWindow>
#include "mainwindow.h"

enum{AMR_xg = 0,AMR_dhb};

namespace Ui {
class MainUI;
}

class MainUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainUI(QWidget *parent = nullptr);
    ~MainUI();


//    void mousePressEvent(QMouseEvent *event) override
//       {
//           if (event->button() == Qt::LeftButton)
//           {
//               // 记录鼠标按下时的全局坐标
//               m_dragStartPosition = event->globalPos();
//           }
//       }

//       void mouseMoveEvent(QMouseEvent *event) override
//       {
//           if (event->buttons() & Qt::LeftButton)
//           {
//               // 计算鼠标移动的偏移量
//               QPoint delta = event->globalPos() - m_dragStartPosition;
//               // 更新窗口的位置
//               move(pos() + delta);
//               // 更新鼠标按下时的坐标
//               m_dragStartPosition = event->globalPos();
//           }
//       }

       MainWindow MainShow;

signals:

       void connecttoTcp(QString ip,QString AMRname,int AMRType);    //通信的槽函数  用于连接AMR

private slots:
       void on_MainUI_Button_clicked();

       void on_Make_Button_clicked();

       void on_TurnPage_Button_clicked();

       void on_Mine_Button_clicked();

       void on_Full_Button_clicked();

private:
    Ui::MainUI *ui;
    QPoint m_dragStartPosition;

};

#endif // MAINUI_H
