#ifndef CAR_TCP_H
#define CAR_TCP_H
#include <QJsonValue>
#include <QMetaType>
#include <QObject>
#include <QThread>
#include <stdint.h>
#include <qhostaddress.h>
#include <QtEndian>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include "getjson.h"
#include <QHostInfo>
#include <QPointF>

class car_tcp : public QObject
{
    Q_OBJECT
public:
    car_tcp();
    void start_listen();
    void stop_listen();
    void send_message(QByteArray str,uint16_t type);

    QVector <MyPoint> path_point;
    QString jsonData;
    QVector <MyPoint> RadarPoint;


signals:
    void SendAMRMes(double x,double y,double yaw);
    void SendRadar(double x, double y);
    void addtheBar(double x,double y);

private slots:
    void do_newConnection();
    void do_socketReadyread();
    void do_clientConnect();
    void Messend();
    void mapget();
    void onDisconnected();

    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *tcpSocket = nullptr;
    QTcpServer *tcpServer;
    QString getLocalIP();
    QWidget *tcp_widget;
    getJson *tcp_map;
    //保存上一次读取的tcp信息
    QByteArray _lastMessage;
};

class XHeaders
{
private:
    /* data */
public:
    uint8_t _sync = 0x5a;                      // 同步头
    uint8_t _fromID;                   // 版本号
    uint16_t _type = 0;                      // 序号   1-运动至节点的指令 2-建图开始 3-建图结束
    uint64_t _length = 0;                      // 数据区长度

    XHeaders(){};
    ~XHeaders(){};
};
#endif // CAR_TCP_H
