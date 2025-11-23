#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QMainWindow>
#include "getjson.h"
#include <QPointF>
namespace Ui {
class TaskWindow;
}

class TaskWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr);
    ~TaskWindow();
    getJson json2;

public slots:
    void getflesh();


private slots:
    void on_Confirm_Button_clicked();

    void on_Cancel_Button_clicked();

private:
    Ui::TaskWindow *ui;
signals:
    void SendTaskinfo(int type,int priority,QString start,int startHigh,QString end,int endHigh,QString source,QPointF startP,QPointF endP);




};

#endif // TASKWINDOW_H
