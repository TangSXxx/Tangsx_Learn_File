#include "foshelper.h"
#include <QFile>
#include <iostream>
#include "WidgetControl/minfoswindow.h"
#include <QDebug>
//日志函数
bool LOGWRITE(const string& content){
    std::cout << content <<endl;
    QFile logfile(LOGFile);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    string result  = "[" + timestamp.toStdString() + "]" + content;

    if(logfile.open(QIODevice::Append | QIODevice::Text)){
        logfile.write(result.c_str());
        logfile.close();
    }else{
       qDebug() << "open log fail" << logfile.errorString() << endl;
        return false;
    }
    return true;
}

