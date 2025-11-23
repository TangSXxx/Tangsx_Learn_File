#ifndef GETJSON_H
#define GETJSON_H
#include <QPointF>
#include <vector>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <QString>
#include <QDebug>
#include <QFile>
enum {
    _2D_Type = 0,
    _3D_Type = 1
};
using namespace std;
struct MyPoint //创建点的数据
{
    double x;
    double y;
};
// 定义结构体表示路径信息
struct Path {
    QPointF start; // 起点坐标
    QPointF end;   // 终点坐标
    QString Start_Name;      //起点名称
    QString End_Name;      //终点名称
};

struct Site {
    QPointF start; // 站点坐标
    QString Site_Name;      //站点名称
    double angle;
};

struct JsonMes{
    QJsonArray advancedCurveList; //路劲json信息
    QJsonArray advancedPointList; //站点json信息
    QJsonObject  header; //基础json信息
    QJsonArray normalPosList;  //障碍物json信息
};

class getJson
{
public:
    getJson();

    //分类起始的json信息
    void ClassifyJson();

    //获取Json站点信息
    void getSite();
    //增加、修改、删除站点
    // 读取 JSON 文件
    QJsonDocument readJsonFile();
    //将json保存至文件
    bool writetoJson(const QJsonDocument& jsonDoc);
    // 在 JSON 中增加一个新的位置信息
    void addLocation(const QString& className, const QString& instanceName, double x, double y,double ag = 0);
    //移除一处的站点信息
    void removeLocation(const QString& instanceName);
    void OperateSite(int mode,const QString className,const QString SiteName,double x = 0,double y = 0,const QString NewSite = NULL);  //0-删除  1-增加  2-修改

    //获取Json路径信息
    void getLine();
    void removeLine(const QString& instanceName);
    void addLine(const QString& className,const QString& start_instanceName,const QString& end_instanceName,QPointF startPos,QPointF endPos);


    //获取Json障碍物信息
    void getBarrier();
    void removeBarrier(double x_left,double y_left,double Length,double Width);
    void addBarrier(double x,double y);

    //获取Json属性信息
    void getMes();
    void reviseMapName(const QString MapName);

    //修改文件地址
    void reviseAdress(QString adress){
        map_address = adress;
    }

    //修改文件地址
    void reviseAddress(QString adress);

    //获取json全部信息
    void getAll();

    //更新AMR扫描的信息
    void AMR_Mes();

    //更新属性信息
    void UpdateMapMes();

    //地图信息
    QVector<Site> Site_vector;
    std::vector<Path> paths;
    QVector<QPointF> Barrier_vector;
    int MapType;

    QPointF MapPos;
    QString MapName;
    QString version;
    QPointF MinPos;

    //json
    JsonMes MapJson;


    QString tcp_message;



    //文件地址
    QString map_address = "./algorithm_map.smap";




};

#endif // GETJSON_H
