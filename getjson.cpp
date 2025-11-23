#include "getjson.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>

getJson::getJson()
{

}
//读取json文件
QJsonDocument getJson::readJsonFile()
{
    QFile file(map_address);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open JSON file22:" << file.errorString();
            return QJsonDocument();
        }

        QString jsonData = file.readAll();
        file.close();

        return QJsonDocument::fromJson(jsonData.toUtf8());
}

// 写入 JSON 文件
bool getJson::writetoJson(const QJsonDocument& jsonDoc) {
    QFile file(map_address);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open JSON file for writing:" << file.errorString();
        return false;
    }

    file.write(jsonDoc.toJson());
    file.close();

    return true;
}

void getJson::addLocation(const QString& className, const QString& instanceName, double x, double y,double ag)
{

    // 将 JSON 字符串转换为 QJsonDocument
    QJsonDocument jsonDoc = readJsonFile();

    QJsonObject locationObj;
        locationObj["className"] = className;
        locationObj["instanceName"] = instanceName;
        QJsonObject posObj;
        posObj["x"] = x;
        posObj["y"] = y;
        posObj["angle"] = ag;
        locationObj["pos"] = posObj;

        QJsonArray advancedPointList = jsonDoc.object()["advancedPointList"].toArray();
        advancedPointList.append(locationObj);

        QJsonObject joc = jsonDoc.object();
        joc["advancedPointList"] = advancedPointList;
        jsonDoc.setObject(joc);
        //qDebug()<<"kk"<<jsonDoc;
        // 将修改后的 JSON 文档写回文件
        if (!writetoJson(jsonDoc)) {
           qDebug() << "Failed to write JSON file.";
        }
}

// 从 JSON 中删除指定的位置信息
void getJson::removeLocation(const QString& instanceName) {
    //读取json文件
   QJsonDocument jsonDoc = readJsonFile();
    QJsonArray advancedPointList = jsonDoc.object()["advancedPointList"].toArray();
    for (int i = 0; i < advancedPointList.size(); ++i) {
        QJsonObject locationObj = advancedPointList.at(i).toObject();
        if (locationObj["instanceName"].toString() == instanceName) {
            advancedPointList.removeAt(i);
            break;
        }
    }
    QJsonObject joc = jsonDoc.object();
    joc["advancedPointList"] = advancedPointList;
    jsonDoc.setObject(joc);

    //qDebug()<<"kk"<<jsonDoc;
    // 将修改后的 JSON 文档写回文件
    if (!writetoJson(jsonDoc)) {
       qDebug() << "Failed to write JSON file.";
    }
}

void getJson::ClassifyJson()
{
      // 将 JSON 字符串转换为 QJsonDocument
      QJsonDocument jsonDoc = readJsonFile();

      // 检查 JSON 文档是否有效
      if (!jsonDoc.isNull()) {
          // 获取 JSON 文档的根对象
          QJsonObject jsonObj = jsonDoc.object();

          // 提取 路线 数组
          MapJson.advancedCurveList = jsonObj["advancedCurveList"].toArray();
          // 处理 advancedCurveList 数组，等待进一步提取信息

          // 提取 站点 数组
          MapJson.advancedPointList = jsonObj["advancedPointList"].toArray();
          // 处理 advancedPointList 数组，等待进一步提取信息

          //提取 基础信息 数组
          MapJson.header = jsonObj["header"].toObject();

          //提取障碍物信息
          MapJson.normalPosList = jsonObj["normalPosList"].toArray();


      } else {
          qWarning() << "Invalid JSON document";
      }
}


void getJson::getSite()
{
    //存储站点之前先清除原先存在的站点
        Site_vector.clear();
        // 遍历 "advancedPointList" 数组
        for (const QJsonValue& value : MapJson.advancedPointList) {
            // 提取数组元素中的对象
            QJsonObject obj = value.toObject();

            // 提取站点名称
            QString siteName = obj["instanceName"].toString();

            // 提取坐标信息
            QJsonObject posObj = obj["pos"].toObject();
            double x = posObj["x"].toDouble();
            double y = posObj["y"].toDouble();
            double angle = posObj["angle"].toDouble();

            // 将坐标信息存储到 Site 结构体中
            Site site;
            site.Site_Name = siteName;
            site.start = QPointF(x, y);
            site.angle = angle;

            // 将 Site 结构体添加到 Site_vector 中
            Site_vector.append(site);
        }
}
void getJson::getLine()
{
    //获取信息前先清除数组中原有的信息
    paths.clear();
    // 遍历 advancedCurveList 数组中的每个对象
        for (const QJsonValue& curveValue : MapJson.advancedCurveList) {
            QJsonObject curveObj = curveValue.toObject();

            // 提取起点和终点信息
            QPointF startPoint(curveObj["startPos"].toObject()["pos"].toObject()["x"].toDouble(),
                               curveObj["startPos"].toObject()["pos"].toObject()["y"].toDouble());
            QPointF endPoint(curveObj["endPos"].toObject()["pos"].toObject()["x"].toDouble(),
                             curveObj["endPos"].toObject()["pos"].toObject()["y"].toDouble());

            // 提取起点和终点名称
            QString startName = curveObj["startPos"].toObject()["instanceName"].toString();
            QString endName = curveObj["endPos"].toObject()["instanceName"].toString();

            // 创建 Path 结构体并添加到容器中
            Path path;
            path.start = startPoint;
            path.end = endPoint;
            path.Start_Name = startName;
            path.End_Name = endName;
            paths.push_back(path);
        }
}

void getJson::getBarrier()
{
    //先清除
    Barrier_vector.clear();
    //循环遍历
    for (const QJsonValue& Barrier : MapJson.normalPosList) {
        QJsonObject BarrierObj = Barrier.toObject();
        //提取坐标
        QPointF pos(BarrierObj["x"].toDouble(),BarrierObj["y"].toDouble());
        //加入最终的容器中
        Barrier_vector.push_back(pos);

        //打印信息
    }
}

void getJson::getMes()
{
    // 从 header 对象中提取信息

        QString mapType = MapJson.header["mapType"].toString();
        double maxX = MapJson.header["maxPos"].toObject()["x"].toDouble();
        double maxY = MapJson.header["maxPos"].toObject()["y"].toDouble();
        double minX = MapJson.header["minPos"].toObject()["x"].toDouble();
        double minY = MapJson.header["minPos"].toObject()["y"].toDouble();
        MapName = MapJson.header["mapName"].toString();
        MinPos = QPointF(minX,minY);
        MapPos = QPointF(maxX, maxY);
        version = MapJson.header["version"].toString();

        // 将提取的信息赋值给对应的变量
        if (mapType == "2D-type") {
            MapType = _2D_Type;
        }
        else if(mapType == "3D-type") {
            MapType = _3D_Type;
        }else {
            // 如果类型不匹配，可以根据实际情况进行处理
            // 这里假设未知类型为 -1
            MapType = -1;
        }
}

void getJson::getAll()
{
    ClassifyJson();
    getLine();
    getSite();
    getBarrier();
    getMes();
}

void getJson::AMR_Mes()
{

}

void getJson::OperateSite(int mode,const QString className,const QString SiteName,double x,double y,const QString NewSite)  //0-删除  1-增加  2-修改
{
    //读取json文件
   QJsonDocument jsonDoc = readJsonFile();

   if(mode == 0)
   {
   // 删除指定的位置信息
   removeLocation(SiteName);
   }
   else if(mode == 1)
   {
   // 增加新的位置信息
   addLocation(className, SiteName, x, y);
   }
   else if(mode == 2)
   {
   // 删除指定的位置信息
   removeLocation(SiteName);
   // 增加新的位置信息
   addLocation(className, NewSite, x, y);
   }

   //qDebug()<<"kk"<<jsonDoc;
   // 将修改后的 JSON 文档写回文件
   if (!writetoJson(jsonDoc)) {
      qDebug() << "Failed to write JSON file.";
   }

}

//删除路线
void getJson::removeLine(const QString& instanceName)
{
    //读取json文件
   QJsonDocument jsonDoc = readJsonFile();

   QJsonArray advancedCurveList = jsonDoc.object()["advancedCurveList"].toArray();
   for (int i = 0; i < advancedCurveList.size(); ++i) {
       QJsonObject locationObj = advancedCurveList.at(i).toObject();
       if (locationObj["instanceName"].toString() == instanceName) {
           advancedCurveList.removeAt(i);
           break;
       }
   }
   QJsonObject joc = jsonDoc.object();
   joc["advancedCurveList"] = advancedCurveList;
   jsonDoc.setObject(joc);

   // 将修改后的 JSON 文档写回文件
   if (!writetoJson(jsonDoc)) {
      qDebug() << "Failed to write JSON file.";
   }

}

void getJson::addLine(const QString& className,const QString& start_instanceName,const QString& end_instanceName,QPointF startPos,QPointF endPos)
{
    //读取json文件
   QJsonDocument jsonDoc = readJsonFile();

   QJsonObject locationObj;
       locationObj["className"] = className;
       locationObj["instanceName"] = QString(start_instanceName + "-" + end_instanceName);
       QJsonObject endPosobj;
       QJsonObject startPosobj;
       QJsonObject propertyobj;
       QJsonObject sposobj;
       QJsonObject eposobj;

       sposobj["x"] = startPos.x();
       sposobj["y"] = startPos.y();
       startPosobj["instanceName"]  = start_instanceName;
       startPosobj["pos"] = sposobj;
       locationObj["startPos"] = startPosobj;

       eposobj["x"] = endPos.x();
       eposobj["y"] = endPos.y();
       endPosobj["instanceName"]  = end_instanceName;
       endPosobj["pos"] = eposobj;
       locationObj["endPos"] = endPosobj;


       QJsonArray propertyArray;
       QJsonObject Object1;
       Object1.insert("key", "direction");
       Object1.insert("type", "int");
       Object1.insert("value", "MA==");
       Object1.insert("int32Value", 0);
       QJsonObject Object2;
       Object2.insert("key", "movestyle");
       Object2.insert("type", "int");
       Object2.insert("value", "MA==");
       Object2.insert("int32Value", 0);
       propertyArray.append(Object1);
       propertyArray.append(Object2);
       locationObj.insert("property",propertyArray);


       QJsonArray advancedCurveList = jsonDoc.object()["advancedCurveList"].toArray();
       advancedCurveList.append(locationObj);

       QJsonObject joc = jsonDoc.object();
       joc["advancedCurveList"] = advancedCurveList;
       jsonDoc.setObject(joc);

       // 将修改后的 JSON 文档写回文件
       if (!writetoJson(jsonDoc)) {
          qDebug() << "Failed to write JSON file.";
       }

}

void getJson::removeBarrier(double x_left,double y_left,double Length,double Width)
{
    //读取json文件
   QJsonDocument jsonDoc = readJsonFile();
   QJsonArray normalPosList = jsonDoc.object()["normalPosList"].toArray();
    for (int i = 0; i < normalPosList.size(); ++i) {
        QJsonObject locationObj = normalPosList.at(i).toObject();
        double x = locationObj["x"].toDouble();
        double y = locationObj["y"].toDouble();
        if ( abs(x-x_left) < Length && abs(y-y_left) < Width) {
            normalPosList.removeAt(i);
        }
    }
    QJsonObject joc = jsonDoc.object();
    joc["normalPosList"] = normalPosList;
    jsonDoc.setObject(joc);

    // 将修改后的 JSON 文档写回文件
    if (!writetoJson(jsonDoc)) {
       qDebug() << "Failed to write JSON file.";
    }
}

void getJson::addBarrier(double x,double y)
{
    // 将 JSON 字符串转换为 QJsonDocument
    QJsonDocument jsonDoc = readJsonFile();

    QJsonObject locationObj;
        locationObj["x"] = x;
        locationObj["y"] = y;

        QJsonArray advancedPointList = jsonDoc.object()["normalPosList"].toArray();
        advancedPointList.append(locationObj);

        QJsonObject joc = jsonDoc.object();
        joc["normalPosList"] = advancedPointList;
        jsonDoc.setObject(joc);
        if (!writetoJson(jsonDoc)) {
           qDebug() << "Failed to write JSON file.";
        }
}

void getJson::reviseAddress(QString address)
{
    map_address = address;
}























