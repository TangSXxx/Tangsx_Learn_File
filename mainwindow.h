#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "getjson.h"
#include <QMainWindow>
#include <car.h>
#include <QString>
#include <qcustomplot.h>
#include "popupnotification.h"
#include <car_tcp.h>
#include "classfile.h"
#include <QtSql>
#include "taskwindow.h"
#include "communicate.h"
#include "car.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum {margin = 5, radius = 30, isNotActive = -2, isActive = -1,  erased = -99, inf = 1000000};
extern int online_ifeer;
enum{};

class Task_wby{   //王碧瑶
public:
  int TaskType;    //   0-移库  1-出库     2-入库 3-无
  int TaskState;   //   0-无    1-第一段   2-正在执行第二段 3-正在执行最后一段  4-已完成（清除完数据 再转换为0）
  Site start,end;
  int startHigh,endHigh;
  QStringList pathRoad;

  void clear(){
        TaskType = 3;
        TaskState = 0;
        pathRoad.clear();
  }

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum{point = 0,line,eraser,normal,dele,NONE};
    int cusormode = NONE;

    getJson json;

    car_tcp* TcpMes; //实验室机器人通信对象
    QThread* tcp_thread;  //通信的线程

    int CurrentIndex = 0;  //机器人初始位置所在索引
    int CurrentAngle = 0;

    QString On_SiteName;
    int On_num = 0;
    CustomTextItem *AMR[12] = {nullptr};

    int CurrrntPotIndex = -1;


    void PaintMap();

    void openTable();   //数据库管理文件

    void addMainBarrier(double x,double y);

    void getCarData(double x,double y,double angle);
    void connect_thread(int num, car* car);
    void connect_modbus(car* car);          //modbus通信初始化
    void connect_WMS(car* car);                     //WMS通信初始化
    void delete_thread(int num);
    void delete_WMS();
    void showModbusConnect(bool mode, bool state);
    void showModbusStatus(bool sta, bool warn);
    void connected_success(QString ip);


protected:
    //dijistra算法
    QMap<int, QVector<int>> matrix; //用于存储图的邻接关系，即哪些节点与其他节点相邻
    QMap<int, QVector<int>> weight;
    QMap<int, QVector<qreal>> angle;
    QVector<int> DijkstraPath; //存储dijistar算法的路径
    QVector<int> BellmanPath;
    QVector<QPointF> position; //存储每个节点的坐标
    QPoint edgeManager; //边的信息
    QPoint DijkstraManager;  //起点和终点
    QPoint BellmanManager;
    
    //仙工AMR通信
    car* amr[12] = {nullptr};
    SendThread* sendthread[12] = {nullptr};
    RecvThread* recvthread[12] = {nullptr};
    QThread* sendtask[12] = {nullptr};
    QThread* recvtask[12] = {nullptr};

    void inital();


private slots:

    void on_action_pot_triggered();

    void on_action_route_triggered();

    void on_action_editmap_triggered();


    void on_action_eraser_triggered();

    void on_action_delete_triggered();

    void on_listWidget_itemPressed(QListWidgetItem *item);

    void on_connect_true_triggered();

    void on_AMRListWidget_itemPressed(QListWidgetItem *item);

    void on_move_map_clicked();

    void on_action_newtask_triggered();

    void on_Delete_Button_clicked();

    void on_TaskStart_Button_clicked();

    void on_action_flush_triggered();

    void on_action_loadmap_triggered();

    void on_PointAngle_edit_textChanged(const QString &arg1);

    void on_SaveButton_clicked();

    void on_pushButton_clicked();

public slots:
    void on_mousePressEvent(QMouseEvent *event);
    void on_mouseMoveEvent(QMouseEvent *event);
    void on_mouseRealseEvent(QMouseEvent *event);
    void on_itemClick(QCPAbstractItem *item, QMouseEvent *event);


    void ConnectIP(QString ip,int type);  //弹窗类的槽函数

    void AcceptTaskinfo(int type,int priority,QString start,int startHigh,QString end,int endHigh,QString source,QPointF startP,QPointF endP);

    //站点位置信号
   // void SendAMRPosP(Site AMRSite,int AMRNum);
    void AcceptAMRMes(double x,double y,double yaw);
    void AcceptRadar(double x,double y);

signals:
    void send_tcp(QByteArray str,uint16_t type);   //实验室相关机器人通信方式
    void start_listen();

protected slots:

    void Dijistra_clear();
    void Dijistra_path();  //单AMR路径规划算法


private:
    Ui::MainWindow *ui;

    int ifpress = 0;

    void handleTimeout(int i); //延时函数

    QSqlDatabase DB;//定义数据库端口
    CustomSqlQueryModel *TaskSQLModel;    //任务信息显示所需要用到的模型
    CustomSqlQueryModel *TaskModel;       //执行任务时，需要读取数据库所用到的模型
    int CurrentSQL = 0;                   //数据库索引
    TaskWindow w;
    popupNotification linktcp;
    QItemSelectionModel *theSelection;
    Task_wby Task_1,Task_2;

    bool MapMove = false;

    void update(); //更新AMR实时位置以及姿态
    void updateplot(); // 更新雷达信息，以及图形信息

    void UpdateMes(); //实时更新属性数据。



};
#endif // MAINWINDOW_H
