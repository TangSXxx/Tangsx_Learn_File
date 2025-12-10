#include "http_server.h"
#include <QDateTime>
#include <QStringList>
#include <QThread>
#include <QDebug>

// 初始化静态成员
HTTP_SERVER* HTTP_SERVER::m_instance = nullptr;
QMutex HTTP_SERVER::m_mutex;

HTTP_SERVER* HTTP_SERVER::instance(QString ip , int port)
{
    QMutexLocker locker(&m_mutex);
    if (m_instance == nullptr) {
        m_instance = new HTTP_SERVER(ip , port);
    }
    return m_instance;
}

HTTP_SERVER::HTTP_SERVER(QString ip ,int port , QObject* parent)
    : QTcpServer(parent), m_port(port), m_isRunning(false) , ipAdress(ip)
{
    // 添加默认路由
    addRoute("/","GET", [this](const QMap<QString, QString>& params) {
        return defaultHandler(params);
    });
}

bool HTTP_SERVER::startServer()
{
    if (m_isRunning) {
        qWarning() << "Server is already running";
        return true;
    }

    if (!listen(QHostAddress(ipAdress), m_port)) {
        qCritical() << "Failed to start server:" << errorString();
        return false;
    }

    m_isRunning = true;
    qInfo() << "HTTP server started on port" << m_port;
    return true;
}

void HTTP_SERVER::stopServer()
{
    if (m_isRunning) {
        close();
        m_isRunning = false;
        qInfo() << "HTTP server stopped";
    }
}

void HTTP_SERVER::addRoute(const QString& path, const QString& method , std::function<QString(const QMap<QString, QString>&)> handler)
{
    QString methods = path + "_" + method;
    m_routes[methods] = handler;
}

void HTTP_SERVER::incomingConnection(qintptr socketDescriptor)
{
    // 直接创建ClientHandler，并传入socketDescriptor
    ClientHandler* handler = new ClientHandler(socketDescriptor, this);
    QThreadPool::globalInstance()->start(handler);
}

QMap<QString, QString> HTTP_SERVER::parseRequest(const QString& request)
{
    QMap<QString, QString> parsed;
    QStringList lines = request.split("\r\n");

    if (lines.isEmpty()) return parsed;

    // 解析请求行
    QStringList requestLine = lines[0].split(" ");
    if (requestLine.size() >= 3) {
        parsed["method"] = requestLine[0];
        parsed["path"] = requestLine[1];
        parsed["http_version"] = requestLine[2];
    }

    // 解析头部
    for (int i = 1; i < lines.size(); ++i) {
        if (lines[i].isEmpty()) break; // 空行表示头部结束

        int colonPos = lines[i].indexOf(":");
        if (colonPos > 0) {
            QString key = lines[i].left(colonPos).trimmed();
            QString value = lines[i].mid(colonPos + 1).trimmed();
            parsed[key.toLower()] = value;
        }
    }

    return parsed;
}

QByteArray HTTP_SERVER::generateResponse(const QString& content, const QString& contentType, int statusCode)
{
    QString statusText;
    switch (statusCode) {
        case 200: statusText = "OK"; break;
        case 404: statusText = "Not Found"; break;
        case 500: statusText = "Internal Server Error"; break;
        default: statusText = "Unknown"; break;
    }

    QString response = QString("HTTP/1.1 %1 %2\r\n"
                              "Content-Type: %3; charset=utf-8\r\n"
                              "Content-Length: %4\r\n"
                              "Connection: close\r\n"
                              "Server: QtHTTP_SERVER/1.0\r\n"
                              "Date: %5\r\n\r\n"
                              "%6")
                      .arg(statusCode)
                      .arg(statusText)
                      .arg(contentType)
                      .arg(content.toUtf8().size())
                      .arg(QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss GMT"))
                      .arg(content);

    return response.toUtf8();
}

QString HTTP_SERVER::defaultHandler(const QMap<QString, QString>& params)
{
    QString path = params.value("path", "/");

    if (path == "/") {
        return "<html><body><h1>Qt HTTP Server</h1><p>Welcome to the home page!</p></body></html>";
    }

    return "<html><body><h1>404 Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
}

void HTTP_SERVER::handleClient(QTcpSocket* clientSocket)
{
    // 等待数据到达
    if (clientSocket->waitForReadyRead(5000)) {
        // 读取请求数据
        QByteArray requestData = clientSocket->readAll();
        QString requestStr = QString::fromUtf8(requestData);

        // 解析请求
        QMap<QString, QString> requestParams = parseRequest(requestStr);
        QString methods =requestParams.value("path", "/")  + "_"  + requestParams.value("method", "/");

        // 查找匹配的路由
        QString responseContent;
        if (m_routes.contains(methods)) {
            try {
                responseContent = m_routes[methods](requestParams);

            } catch (const std::exception& e) {
                qWarning() << "Error in route handler:" << e.what();
                responseContent = "<html><body><h1>500 19 Server Error</h1><p>An error occurred while processing your request.</p></body></html>";
            }
        } else {
            // 使用默认处理程序
            responseContent = defaultHandler(requestParams);
        }

        // 生成并发送响应
        QByteArray response = generateResponse(responseContent);
        clientSocket->write(response);
        clientSocket->waitForBytesWritten(1000);
    }

}

// 实现
ClientHandler::ClientHandler(qintptr socketDescriptor, HTTP_SERVER* server)
    : m_socketDescriptor(socketDescriptor), m_server(server)
{
    setAutoDelete(true);
}

void ClientHandler::run()
{
    // 在工作线程中创建QTcpSocket
    QTcpSocket socket;
    if (!socket.setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Error setting socket descriptor:" << socket.errorString();
        return;
    }

    // 现在调用HttpServer的handleClient函数，传入这个套接字
    m_server->Callhandclient(&socket);
}
