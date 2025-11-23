#pragma once
#include <QtNetwork>
#include <QTcpServer>
#include <QLabel>
#include <unordered_map>


class TcpCLient : public QObject{
   Q_OBJECT

public:
    QTcpSocket* tcpSocket;
    QString ClientIP;
    QString ClientPort;
    TcpCLient(QObject* parent = nullptr) : QObject(parent), tcpSocket(nullptr) {}
    ~TcpCLient();

public slots:
    void  onSocketStateChange(QAbstractSocket::SocketState socketState);
    void  onClientConnected(); //Client Socket connected
    void  onClientDisconnected();//Client Socket disconnected
    void  onSocketReadyRead();//读取socket传入的数据

};

class FOSTcpServer : public QObject {
    Q_OBJECT

public:
    FOSTcpServer& operator=(const FOSTcpServer&) = delete;
    static FOSTcpServer* instance();

    QTcpServer *tcpServer; //TCP服务器
    std::unordered_map<std::string, TcpCLient*> ServerVec;

    QString getLocalIP();//获取本机IP地址
    void startListen(QString ip , quint16 port);
    void stopListen();

    ~FOSTcpServer();

private:
    static FOSTcpServer* m_instance;
    FOSTcpServer();

private slots:
    void onNewConnection();
};
