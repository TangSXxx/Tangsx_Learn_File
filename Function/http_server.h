#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QMutex>
#include <QThreadPool>
#include <QRunnable>

class HTTP_SERVER : public QTcpServer
{
    Q_OBJECT

public:
    // 获取单例实例
    static HTTP_SERVER* instance(QString ip , int port = 8080);

    // 启动服务器
    bool startServer();

    // 停止服务器
    void stopServer();

    // 添加路由处理函数
    void addRoute(const QString& path, const QString& method , std::function<QString(const QMap<QString, QString>&)> handler);

    void Callhandclient(QTcpSocket* clientSocket){
        handleClient(clientSocket);
    };

protected:
    // 重写 incomingConnection 方法
    void incomingConnection(qintptr socketDescriptor) override;

private:
    // 私有构造函数，实现单例模式
    explicit HTTP_SERVER(QString ip  , int port , QObject* parent = nullptr);

    // 删除拷贝构造函数和赋值运算符
    HTTP_SERVER(const HTTP_SERVER&) = delete;
    HTTP_SERVER& operator=(const HTTP_SERVER&) = delete;

    // 解析HTTP请求
    QMap<QString, QString> parseRequest(const QString& request);

    // 生成HTTP响应
    QByteArray generateResponse(const QString& content, const QString& contentType = "text/html", int statusCode = 200);

    // 处理客户端请求
    void handleClient(QTcpSocket* clientSocket);

    static HTTP_SERVER* m_instance;
    static QMutex m_mutex;

    int m_port;
    QString ipAdress;
    bool m_isRunning;

    // 路由表：路径到处理函数的映射
    QMap<QString, std::function<QString(const QMap<QString, QString>&)>> m_routes;

    // 默认路由处理函数
    QString defaultHandler(const QMap<QString, QString>& params);
};

// 用于处理客户端请求的runnable类
// 在ClientHandler类中，修改构造函数和成员变量
class ClientHandler : public QRunnable
{
public:
    ClientHandler(qintptr socketDescriptor, HTTP_SERVER* server);
    void run() override;

private:
    qintptr m_socketDescriptor;
    HTTP_SERVER* m_server;
};



#endif // HTTP_SERVER_H
