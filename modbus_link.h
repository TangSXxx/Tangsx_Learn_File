#ifndef MODBUS_LINK_H
#define MODBUS_LINK_H


#include <QObject>
#include <QMap>
#include <QTimer>
#include "car.h"
#include <Ws2tcpip.h>
#define M_base 320

enum{success = 0,faild};
enum{turnleft = 0,turnright};

class ModbusThread : public QObject
{
    Q_OBJECT
public:
    ModbusThread(QString ip, car* car);
    modbus_t *mb;
    car* m_car;
    QString m_ip;
    QTimer* loop_timer;
    void connectModbus();
    void disconnectModbus();
    void writeCommand(int addr,bool state);
    QMap<QString, int> listpoint;

    void write_shelfTask(int point, int high, bool mode);
    //上层机构指令对应数组 0-27为货架取送货 28-37为背篓取送货 38-39为接驳台取送货
    int modebusList[43] = {344,345,346,347,348,394,396,349,350,351,352,353,395,397,\
                           360,361,362,363,364,398,399,365,366,367,368,369,400,401,\
                          355,356,357,358,359,\
                          371,372,373,374,375,\
                           354,370,\
                           417,416,327};

    void write_basketTask(int high, bool mode);
    void write_stationTask(bool mode);
    void write_warnReset();
    void write_motionCancel();
    void write_motionReset();
    void read_status_loop();

signals:
    void connectState(bool mode, bool state=0);
    void start_loop();
    void delete_thread();
    void send_status(bool sta,bool warn);
};

#endif // MYMODBUS_H
