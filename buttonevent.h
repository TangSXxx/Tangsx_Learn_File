#ifndef BUTTONEVENT_H
#define BUTTONEVENT_H
#include "WidgetControl/tabwidgetcon.h"
#include <QDebug>
class ButtonEvent
{

private:
    QString IpAdress;
    int Port;
    TabWidgetCon* tabObj = nullptr;

public:
    ButtonEvent(const QString& ip , const int& port,TabWidgetCon* tab = nullptr) : IpAdress(ip) , Port(port) , tabObj(tab){
        qDebug() << "ip:" << ip << "port" << port;
    };

public slots:
    void AddButtonEvent();
    void DeleteButtonEvent(int index);
    void StartButtonEvent();
};

#endif // BUTTONEVENT_H
