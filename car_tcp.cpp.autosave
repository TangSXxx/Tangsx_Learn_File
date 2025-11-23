#include "car_tcp.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QRegularExpression>
#include <QPointF>

car_tcp::car_tcp()
{
    tcpServer = new QTcpServer(this);
    tcp_widget= new QWidget();
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(do_newConnection()));
    qDebug()<<"eeeee:"<<QThread::currentThreadId();
}


QString car_tcp::getLocalIP()
{
    QString localHostName = QHostInfo::localHostName();
    QHostInfo hostInfo = QHostInfo::fromName(localHostName);
    QString local_ip = " ";
    QList<QHostAddress> addrList = hostInfo.addresses();
    if(addrList.isEmpty())
        return local_ip;
       foreach(QHostAddress aHost, addrList)
       {
           if(aHost.protocol() == QAbstractSocket::IPv4Protocol)
           {
              local_ip = aHost.toString();
              break;
           }
       }
       return local_ip;
}
void car_tcp::send_message(QByteArray str, uint16_t type = 0)
{
    qDebug() << "send_message str"<<str;
    qDebug() << "Before send_message - State:" << tcpSocket->state();
    qDebug()<<"send_message线程:"<<QThread::currentThreadId();
    int data_size = str.length() + 16;
    qDebug() << "字符串长度: "<<data_size;
    char* msg = (char*)malloc(data_size);
    if (msg == NULL)
        {
            qDebug() << "数据包内存分配失败!";
            return;
        }
        XHeaders *_header = new XHeaders;
        _header->_type = type;
        _header->_length = str.length();
        memset(msg, 0, data_size);
        memcpy(msg, _header, 16);
        if (!str.isEmpty())
        {
            memcpy((msg+16), str.data(), _header->_length);
        }
        //QByteArray test = QByteArray(msg);
        int send_size = tcpSocket->write(msg,data_size);
        qDebug()<<"发送的信息长度为："<<send_size;

        qDebug() << "send_message msg"<<msg;
        delete _header;
        free(msg);
        qDebug()<<"tcp状态："<<tcpSocket->state();
}

void car_tcp::start_listen()
{
    /*QString IP = "127.0.0.1";
    QHostAddress address(IP);*/
    int port = 8080;
    tcpServer->listen(QHostAddress::Any,port);
    qDebug()<<"start_listen:"<<QThread::currentThreadId();
}
void car_tcp::stop_listen()
{
    if(tcpServer->isListening())
    {
        if(tcpSocket != nullptr)
            if(tcpSocket->state()==QAbstractSocket::ConnectedState)
                tcpSocket->disconnectFromHost();
        tcpServer->close();
    }
}

void car_tcp::do_newConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
    bool success = connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(do_socketReadyread()));
    connect(tcpSocket, &QTcpSocket::disconnected, this, &car_tcp::onDisconnected);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &car_tcp::onError);

    connect(tcpSocket,SIGNAL(connected()),this,SLOT(do_clientConnect()));
    qDebug()<<"有新的客户端连接:"<<QThread::currentThreadId();
}

void car_tcp::onDisconnected() {
    qDebug() << "Socket disconnected.";
}

void car_tcp::onError(QAbstractSocket::SocketError socketError) {
    qDebug() << "Socket error:" << tcpSocket->errorString();
}
void car_tcp::do_socketReadyread()
{
    qDebug()<<"do_socketReadyread:"<<QThread::currentThreadId();
    online_ifeer = 1;
    QByteArray message = tcpSocket->readAll();
    if (message.isEmpty()) {
            qDebug() << "No data available to read.";
            return;
        }

    qDebug()<<"tcp test:"<<message.length()<<'\n';
    message = _lastMessage + message;
    qDebug()<<"拼接后数据的长度"<<message.size();
    int size = message.size();
    qDebug()<<"程序读取的 message 长度"<<message.size()<<endl;
    if(size > 0){
        if (message.contains('Z')){//检测第一位是否为0x5A.
            message = message.mid(message.indexOf('Z'));
            if (size >= 16) {//返回的数据最小长度为16位,如果大小小于16则数据不完整等待再次读取.
                XHeaders* header = new XHeaders;
                memcpy(header, message.data(), 16);
                qDebug()<<"数据区头部 16 字节的数据"<<header;

                uint64_t data_size = header->_length;//返回数据区总长值.
                uint16_t data_type = header->_type;
                //qToBigEndian(header->_length,(uint8_t*)&(data_size));
                delete header;

                uint32_t remaining_size = size - 16;//已读数据总长度 - 头部总长度16 = 现有数据区长度.
                //如果返回数据长度值 大于 已读取数据长度 表示数据还未读取完整，跳出循环等待再次读取..
               if (data_size > remaining_size) {
                    _lastMessage.clear();
                    _lastMessage = message;
                    return;
                }

                    //截取报头16位后面的数据区数据.
                    QByteArray json_data = message.mid(16,data_size);

                    _lastMessage.clear();
                    _lastMessage = message.mid(16+data_size);

                    QString json_string = QString(json_data);
                    jsonData = json_string;
//                    if(data_type == 2){
//                        mapget();
//                    }
//                    else{
//                        Messend();
//                    }
                   Messend();


            } else{
                qDebug()<<"error!帧头不完整";
            }
        }else{
            //报头数据错误.
            qDebug()<<"error!帧首不对";
        }
    }
}
void car_tcp::do_clientConnect()
{

}

void car_tcp::Messend()
{
    QVector<MyPoint> v_point;
    MyPoint cur_pose;

    v_point.clear();
    RadarPoint.clear();
    double car_angle = 0;;

    QStringList user_data1;
    QStringList user_data2;
    MyPoint z;

    if(online)
    {
            int path_place = jsonData.indexOf(" path");
            int x_place = jsonData.indexOf("x");
            int y_place = jsonData.indexOf("y");
            int laser_place = jsonData.indexOf("laser");
            qDebug()<<"x_place: "<<x_place<<"laser_place: "<<laser_place<<"path_place: "<<path_place;
            if(path_place!=-1 && x_place!=-1 && y_place!=-1 && laser_place!=-1)
            {
                user_data1=jsonData.left(path_place).split(" ");
                for(int i=0;i<user_data1.size();i++)
                {
                        if(i==0||i==2||i==4)
                        {
                            continue;
                        }
                        if(i==1)
                        {
                            cur_pose.x=user_data1[1].toDouble();
                           qDebug() << "cur_pose.x"<<user_data1[1].toDouble();
                        }
                        else if(i==3)
                        {
                            cur_pose.y=user_data1[3].toDouble();
                            qDebug() << "cur_pose.y"<<user_data1[3].toDouble();

                        }
                        else if(i==5)
                        {
                            car_angle=user_data1[5].toDouble();
                            qDebug() << "car_yaw"<<car_angle;
                            i++;
                        }
                        else
                        {
                            if(i%2==1)
                            {
                                z.x=user_data1[i].toDouble();
                            }
                            else
                            {
                                z.y=user_data1[i].toDouble();

                                //qDebug() << "z.x"<<z.x<<"z.y"<<z.y;
                            }
                            v_point.push_back(z);
                            RadarPoint.push_back(z);
                        }

                 }

                if(path_place!=0)
                {
                    user_data2=jsonData.right(jsonData.size()-path_place-5).split(" ");
                    qDebug()<<jsonData.right(jsonData.size()-path_place-5);
                    for(int i=1;i<user_data2.size();i++)
                    {
                                if(i%2!=1)
                                {
                                    z.x=user_data2[i].toDouble();
                                }
                                else
                                {
                                    z.y=user_data2[i].toDouble();
                                    qDebug() << "path_point.x"<<z.x<<"path_point.y"<<z.y;
                                }
                     }
                }

            }
            else{
                //qDebug() << "错误的";
            }
    emit SendAMRMes(cur_pose.x,cur_pose.y,car_angle);
    }
}

void car_tcp::mapget()
{
    qDebug()<<"建图函数运行中："<<jsonData<<endl;
    // Regular expression to match pose and map with their coordinates
    QStringList strList = jsonData.split(" ",QString::SkipEmptyParts);
    if(strList[0] == "map" ){
        double x = strList[1].toDouble();
        double y = strList[2].toDouble();
        addtheBar(x,y);
    }
    else if(strList[0] == "pose"){
        double x = strList[1].toDouble();
        double y = strList[2].toDouble();
        qDebug()<<"更新坐标："<<x<<y<<endl;
        emit SendAMRMes(x,y,0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//   通信线程相关
/////////////////////////////////////////////////////////////////////////////////////////////


















