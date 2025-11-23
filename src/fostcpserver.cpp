#include "fostcpserver.h"
#include "minfoswindow.h"
#include "foshelper.h"
#include <QDebug>
// 初始化静态成员
FOSTcpServer* FOSTcpServer::m_instance = nullptr;

//构造函数
FOSTcpServer::FOSTcpServer(){
    tcpServer = new QTcpServer();
    connect(tcpServer, &QTcpServer::newConnection, this, &FOSTcpServer::onNewConnection);
}

FOSTcpServer::~FOSTcpServer()
{
    if(tcpServer) delete tcpServer;
    for(auto obj : ServerVec) delete obj.second;
}

void FOSTcpServer::startListen(QString ip, quint16 port)
{
    QHostAddress addr(ip);
    tcpServer->listen(addr,port);
    QString content = "开始监听：" + ip + " " + QString::number(port);
    MinFOSWindow::WriteMes(content);
}

void FOSTcpServer::stopListen()
{
    if (tcpServer->isListening()) //tcpServer正在监听
    {
        tcpServer->close();//停止监听
        MinFOSWindow::WriteMes("停止监听");
    }
}

FOSTcpServer* FOSTcpServer::instance()
{
    if (!m_instance) {
        m_instance = new FOSTcpServer();
    }
    return m_instance;
}


QString FOSTcpServer::getLocalIP()
{
    QString hostName=QHostInfo::localHostName();//本地主机名
    QHostInfo hostInfo=QHostInfo::fromName(hostName);
    QString localIP="";

    QList<QHostAddress> addList = hostInfo.addresses();//

    if (!addList.isEmpty())
    for (int i=0;i<addList.count();i++)
    {
        QHostAddress aHost = addList.at(i);
        if (QAbstractSocket::IPv4Protocol == aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    }
    return localIP;
}

void FOSTcpServer::onNewConnection()
{
    QTcpSocket* tcpSocket = tcpServer->nextPendingConnection();
    if (!tcpSocket) return;

    // 创建新的客户端对象
    TcpCLient* client = new TcpCLient(this);
    client->tcpSocket = tcpSocket;
    client->ClientIP = tcpSocket->peerAddress().toString();
    client->ClientPort = QString::number(tcpSocket->peerPort());

    std::string key = tcpSocket->peerAddress().toString().toStdString();
    ServerVec[key] = client;

    // 连接信号槽
    connect(tcpSocket, &QTcpSocket::disconnected,
           client, &TcpCLient::onClientDisconnected);
    connect(tcpSocket, &QTcpSocket::readyRead,
           client, &TcpCLient::onSocketReadyRead);
    connect(tcpSocket, &QTcpSocket::connected,
           client, &TcpCLient::onClientConnected);
    connect(tcpSocket, &QTcpSocket::stateChanged,
           client, &TcpCLient::onSocketStateChange);

    client->onClientConnected();
    client->onSocketStateChange(tcpSocket->state());
}

TcpCLient::~TcpCLient()
{
    if(tcpSocket) delete tcpSocket;
}

void TcpCLient::onSocketStateChange(QAbstractSocket::SocketState socketState)
{

}

void TcpCLient::onClientConnected()
{
    MinFOSWindow::WriteMes("peer address: " + tcpSocket->peerAddress().toString() + " peer port: " + QString::number(tcpSocket->peerPort()));
}

void TcpCLient::onClientDisconnected()
{
    MinFOSWindow::WriteMes("客户端断链 " + tcpSocket->peerAddress().toString());
}

void TcpCLient::onSocketReadyRead()
{
    while(tcpSocket->canReadLine())
        MinFOSWindow::WriteMes( "发送的数据： " + tcpSocket->readLine());
}



