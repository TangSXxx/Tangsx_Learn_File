#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <QJsonValue>
#include <QMetaType>
#include <QObject>
#include <QThread>
#include <stdint.h>
#include <qhostaddress.h>
#include <QtEndian>
#include <QTcpSocket>
#include "XStateClient.h"
#include "car.h"
#include <QTimer>


enum{Ctrl = 0, Navi, Config};
enum{I_AMR_B1 = 0, I_AMR_B2, I_AMR_D1, I_AMR_D2, WMS = 9};
enum{Out = 0, In};

//小车状态接收线程
class RecvThread : public QObject
{
    Q_OBJECT
public:
    RecvThread(car* car);
    void connectRecv(QString ip);
    void disconnectRecv(int num);
    void ThreadconnectWMS();
    void disconnectWMS();
    void Recv_mission_loop();
    void Recv_loop();
    QString recv_mision();
    void write_data(quint16 sendCommand, const QByteArray &sendData, quint16 number);
    QString recv_data();
    void stopThread();

    XStateClient* xt;       //接收小车位置信息
    QTcpSocket* recv_tcp;
    QTimer * loop_timer;//代码定时器
    bool loop_tricker = 0;
    bool read_ok = 0;

    QString get_location();
    QString get_task_status();
    QString get_battery();
    void get_currentmap();          //获取当前地图名称



signals:
    void recv_value(double x,double y,double angle);
    void send_json(QByteArray json);
    void send_mission(QString get, QString date,QString num,QString kind,QString start,QString starthigh,QString end,QString endhigh,QString priority);
    void stopWMS();
    void send_id(QString ip);
    void got_currentmap();

public slots:
    void thread_error(QAbstractSocket::SocketState);

private:

    QString m_ip;
    car* m_car;     //维护仙工车对象
    QByteArray _lastMessage;

    bool  m_Paused=true;
    bool  m_stop=true;
    bool  thread_start = false;
};

//小车指令发送线程
class SendThread : public QObject
{
    Q_OBJECT

public:
    explicit SendThread(car* car, QObject *parent = nullptr);
    ~SendThread();
    void disconnectSend(int num);
    void write_data(quint16 sendCommand, int mode, const QByteArray &sendData, quint16 number);
    QString recv_data(QTcpSocket* tcp);
    void send_move_task(QStringList movelist);
    void get_send_tasklist(QStringList tasklist);
    void connectSend(QString ip);

    void send_ctrl_motion(double vx, double w, double time = 0);
    void config_motion_par(double maxspeed, double maxwspeed, double maxacc, double maxwacc);
    void stop_ctrl_motion();
    void send_car_pause();
    void send_car_resume();
    void send_car_cancel();

    void send_car_terminal(int pot,int mode, int frame_floor, int car_floor);
    void send_car_terminal_roll(int pot,int mode);
    void send_car_point(QString point);
    void send_car_reset();
    void get_control();
    void release_control();
    void clean_errors();
    void get_map();

private:
    car* send_car;      //维护仙工车对象
    QTcpSocket* navi_tcp;
    QTcpSocket* ctrl_tcp;
    QTcpSocket* config_tcp;
    int send_id;
    QByteArray _lastMessage;

signals:
    void stop_thread(int num);
    void start_drawmap(QString map);

};

#endif // COMMUNICATE_H

