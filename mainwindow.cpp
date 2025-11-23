#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "getjson.h"
#include <QDateTime>
#include <QDebug>
#include <QPointF>
#include <QVector>
#include <vector>
#include <Ws2tcpip.h>
#include <QtMath> // 包含qAtan2和其他数学函数
#include <QPixmap>
#include "mainui.h"
#include "classfile.h"
#include "taskwindow.h"
#include "car_tcp.h"
QCPGraph * curGraph;
QCPItemText *CurrentItem;
std::mutex mtx;
int online_ifeer = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Shortcuts_Bar->setVisible(false);

    openTable(); //打开数据库文件

    ui->Mapwidget->setInteraction(QCP::iRangeDrag, true); //鼠标单击拖动
    ui->Mapwidget->setInteraction(QCP::iRangeZoom, true); //滚轮滑动缩放
    ui->Mapwidget->setInteraction(QCP::iSelectPlottables,true);
    ui->Mapwidget->setInteraction(QCP::iSelectItems ,true);
    ui->Mapwidget->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
    ui->Mapwidget->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));
    ui->Mapwidget->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));
    ui->Mapwidget->xAxis->grid()->setSubGridVisible(false);     // 显示子网格线

    //设置时段控制的参数
    ui->TaskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->TaskTable->horizontalHeader()->setMinimumHeight(50);            //设置表头行高
    ui->TaskTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;color:black;font-weight:bold;}");       //设置表头字体格式
    ui->TaskTable->setAlternatingRowColors(true);
    ui->TaskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TaskTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->TaskTable->setSelectionModel(theSelection);



    json.getAll();
    PaintMap();
    TcpMes = new car_tcp(); //初始化通信对象
    tcp_thread = new QThread;
    TcpMes->moveToThread(tcp_thread);
    qDebug()<<"主线程:"<<QThread::currentThreadId();
    qRegisterMetaType<uint16_t>("uint16_t");
    tcp_thread->start();

    connect(this,&MainWindow::send_tcp,TcpMes,&car_tcp::send_message);
    connect(this,&MainWindow::start_listen,TcpMes,&car_tcp::start_listen);
    connect(TcpMes,&car_tcp::SendAMRMes,this,&MainWindow::AcceptAMRMes);
    connect(TcpMes,&car_tcp::SendRadar,this,&MainWindow::AcceptRadar);
    connect(TcpMes,&car_tcp::addtheBar,this,&MainWindow::addMainBarrier);
    connect(ui->Mapwidget, &QCustomPlot::mouseMove, this, &MainWindow::on_mouseMoveEvent);
    connect(ui->Mapwidget, &QCustomPlot::mousePress, this, &MainWindow::on_mousePressEvent);
    connect(ui->Mapwidget, &QCustomPlot::mouseRelease, this, &MainWindow::on_mouseRealseEvent);
    connect(&w,SIGNAL(SendTaskinfo(int,int,QString,int,QString,int,QString,QPointF,QPointF)),this,SLOT(AcceptTaskinfo(int,int,QString,int,QString,int,QString,QPointF,QPointF)));  //新建任务的弹窗
    connect(&linktcp,SIGNAL(SendConnect(QString,int)),this,SLOT(ConnectIP(QString,int)));  //新建任务的弹窗
    AMR[0] = new CustomTextItem(ui->Mapwidget,-0.2277, 0.0169, 45, "+->");

}

MainWindow::~MainWindow()
{
    delete ui;
}

//更新雷达以及小车信息
void MainWindow::AcceptAMRMes(double x,double y,double yaw)
{
        qDebug()<<"程序运行"<<endl;
        AMR[0]->setItemPos(x,y);
        AMR[0]->setItemRotation(-yaw * 57.3);
        CurrentAngle = yaw;

        for(int i = 0; i < position.size();i++){
            if(abs(position[i].x() - x) <= 0.2 && abs(position[i].y() - y) <= 0.2){
                CurrentIndex = i;
                QString str = QString("%1").arg(json.Site_vector[i].Site_Name);
                ui->statusbar->showMessage(str);
            }
        }
        qDebug()<<"size： "<<TcpMes->RadarPoint.size();
        if(TcpMes->RadarPoint.size() == 0){
            qDebug()<<"1213";
            return;
        }
        QVector<double> rax(TcpMes->RadarPoint.size()),ray(TcpMes->RadarPoint.size());
        for(int i = 0; i < TcpMes->RadarPoint.size(); i++){
            rax[i] = TcpMes->RadarPoint[i].x;
            ray[i] = TcpMes->RadarPoint[i].y;
        }
        QPen mypen1;
        mypen1.setWidth(1);
        mypen1.setColor(Qt::green);
        QPen mypen2;
        mypen2.setWidth(1);
        mypen2.setColor(Qt::blue);
        ui->Mapwidget->addGraph();
        ui->Mapwidget->graph(1)->setPen(mypen1);
        ui->Mapwidget->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->Mapwidget->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssSquare, 0.6));
        ui->Mapwidget->graph(1)->addData(rax,ray);
        ui->Mapwidget->graph(1)->setAdaptiveSampling(true);
        ui->Mapwidget->replot();
        ui->Mapwidget->legend->setVisible(false);
        ui->Mapwidget->removeGraph(1);
        qDebug()<<"位置更新:"<<x<<y<<" 姿态："<<yaw;
}

void MainWindow::AcceptRadar(double x, double y)
{
}

/*
 菜单栏槽函数 编辑地图的功能按钮
     1、添加站点
     2、添加路线
     3、擦除站点
     4、擦除障碍物
*/

//一旦启动编辑地图按钮，侧边工具栏自动弹出 地图成为“可编辑状态”
void MainWindow::on_action_editmap_triggered()
{
    if(ui->action_editmap->text() == "启动编辑")
    {
        ui->action_editmap->setText("退出编辑");
        ui->Shortcuts_Bar->setVisible(true);
        cusormode = normal;
    }
    else
    {
        ui->action_editmap->setText("启动编辑");
        ui->Shortcuts_Bar->setVisible(false);
        ui->Mapwidget->setCursor(Qt::ArrowCursor);
        cusormode = NONE;
    }
}

void MainWindow::on_action_pot_triggered()
{
    if(ui->action_pot->isChecked())
    {
    cusormode = point;
    ui->Mapwidget->setCursor(Qt::CrossCursor);
    }
    else
    {
    cusormode = normal;
    ui->Mapwidget->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::on_action_route_triggered()
{
    if(ui->action_route->isChecked())
    {
    cusormode = line;
    ui->Mapwidget->setCursor(Qt::SizeVerCursor);
    }
    else
    {
    cusormode = normal;
    ui->Mapwidget->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::on_action_eraser_triggered()
{
    if(ui->action_eraser->isChecked())
    {
    cusormode = eraser;
    ui->Mapwidget->setCursor(Qt::SizeVerCursor);
    }
    else
    {
    cusormode = normal;
    ui->Mapwidget->setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::on_action_delete_triggered()
{
   QList<QCPAbstractItem*> selectedItem = ui->Mapwidget->selectedItems();
   foreach(QCPAbstractItem* item, selectedItem) {
   QCPItemText* textItem = qobject_cast<QCPItemText*>(item);
   if(textItem){
   QString name = textItem->text();
   json.removeLocation(name);
   }
   }
   PaintMap();
}

void MainWindow::on_move_map_clicked()
{    
    if(MapMove)
    {
        MapMove = false;
        ui->Mapwidget->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
        ui->move_map->setStyleSheet ("background-color: transparent;");
    }
    else
    {
        MapMove = true;
        ui->Mapwidget->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        ui->move_map->setStyleSheet ("background-color: rgb(255, 255, 127);");
    }}

void MainWindow::on_action_newtask_triggered()  //新建任务
{
    w.show();
    w.getflesh();
}

void MainWindow::on_connect_true_triggered()
{
    linktcp.show();
}
//刷新功能
void MainWindow::on_action_flush_triggered()
{
    PaintMap();
}
//建图功能
void MainWindow::on_action_loadmap_triggered()
{
    //第一步：下发建图的指令（判断通信状态）
    if(ui->action_loadmap->text() == "开始建图"){
        emit send_tcp("map start",2);
        //弹窗显示
        QMessageBox::information(this,"建图","建图指令下发成功");
        ui->action_loadmap->setText("建图中");
    }
    else if(ui->action_loadmap->text() == "建图中"){
        emit send_tcp("map end",3);
        PaintMap();
        //弹窗显示
        QMessageBox::information(this,"建图","建图已结束");
        ui->action_loadmap->setText("开始建图");
    }
}



/*
 鼠标事件
 目标：1、实现鼠标移动站点
      2、选中listwidget中相应条目，可以实现对应站点的操作
      3、自主构建路线
      4、删除障碍物
*/
void MainWindow::on_mousePressEvent(QMouseEvent *event)
{
    // 获取鼠标点击的坐标（相对于界面）
   QPoint mousePressPos = event->pos();

   // 将界面坐标转换为图表坐标
   double xValue = ui->Mapwidget->xAxis->pixelToCoord(mousePressPos.x());
   double yValue = ui->Mapwidget->yAxis->pixelToCoord(mousePressPos.y());

   // qDebug()<<"鼠标按下事件";
    if(cusormode == point){   //如果处于点模式
        if(On_SiteName == "") On_SiteName = "TW";
        QString name = QString("%1%2").arg(On_SiteName).arg(On_num+1);
        json.addLocation("LocationdMark",name,xValue,yValue);
        qDebug()<<"站点构建"<<On_SiteName<<name<<xValue<<yValue;

    }
    if(cusormode == line)
    {
       // json.addLine("StraightPath","LM23","LMJH",QPointF(0.231,0.2345),QPointF(1.22324,2.223));

        // 获取鼠标位置下的项
        QCPAbstractItem *itemUnderCursor = ui->Mapwidget->itemAt(event->pos());
        if (itemUnderCursor) {
            // 检查当前项是否是文本项（QCPItemText）
            if (QCPItemText *textItem = dynamic_cast<QCPItemText*>(itemUnderCursor)) {

                QPointF position = textItem->position->coords();
                QString text = textItem->text();
                CurrentItem = textItem;
                if(event->modifiers() & Qt::ControlModifier)
                {
                    QList<QCPAbstractItem*> selectedItem = ui->Mapwidget->selectedItems();
                            foreach(QCPAbstractItem* item, selectedItem) {
                            QCPItemText* textItem22 = qobject_cast<QCPItemText*>(item);
                            json.addLine("StraightPath",textItem22->text(),textItem->text(),textItem22->position->coords(),textItem->position->coords());
                            }
                }
                // 输出坐标和文字
                qDebug() << "Text item clicked at coordinates:" << position;
                qDebug() << "Text item text:" << text;
            } else {
                qDebug() << "Current item is not a text item.";
            }
        } else {
            qDebug() << "No item under cursor.";
        }


    }
    else if(cusormode == eraser)
    {
        json.removeBarrier(xValue,yValue,0.3,0.3);
        qDebug()<<"删除障碍物："<<xValue<<yValue<<"0.1 0.1";
    }
    else{
        // 获取鼠标位置下的项
        QCPAbstractItem *itemUnderCursor = ui->Mapwidget->itemAt(event->pos());
        if (itemUnderCursor) {
             if (QCPItemText *textItem = dynamic_cast<QCPItemText*>(itemUnderCursor)) {
                 QString text = textItem->text();
                 ui->PointName_Edit->setText(text);

                 for(int i = 0; i < json.Site_vector.size(); i++){
                     if(json.Site_vector[i].Site_Name == text){
                         QString str = QString("%1").arg(json.Site_vector[i].angle);
                         QString str2 = QString("%1").arg(json.Site_vector[i].start.x());
                         QString str3 = QString("%1").arg(json.Site_vector[i].start.y());
                         ui->PointAngle_edit->setText(str);
                         ui->PointX_edit->setText(str2);
                         ui->PointY_edit->setText(str3);
                         CurrrntPotIndex = i;
                     }
                 }
             }

        }

    }
    if(cusormode != NONE)
    {
    PaintMap();
    }

}

void MainWindow::on_mouseMoveEvent(QMouseEvent *event)
{
//    qDebug()<<"鼠标移动事件";
    // 获取鼠标点击的坐标（相对于界面）
   QPoint mousePressPos = event->pos();

   // 将界面坐标转换为图表坐标
   double xValue = ui->Mapwidget->xAxis->pixelToCoord(mousePressPos.x());
   double yValue = ui->Mapwidget->yAxis->pixelToCoord(mousePressPos.y());
    if(cusormode == normal && event->buttons() & Qt::LeftButton)
    {
        QList<QCPAbstractItem*> selectedItem = ui->Mapwidget->selectedItems();
        foreach(QCPAbstractItem* item, selectedItem) {
        QCPItemText* textItem = qobject_cast<QCPItemText*>(item);
        if(textItem){
        QString name = textItem->text();
        qDebug()<<"正在移动中。。。"<<name<<xValue;
        textItem->position->setCoords(xValue,yValue);
        ui->Mapwidget->replot();
        ifpress = 1;
        }
        }

    }
    if(cusormode == eraser)
    {
        json.removeBarrier(xValue,yValue,0.01,0.01);
    }
    else if(cusormode == line)
    {
//        if(linestg == 1)
//        {
//            ssssline->start->setCoords(CurrentItem->position->coords()); // 设置起点坐标
//            ssssline->end->setCoords(QPointF(xValue,yValue)); // 设置终点坐标
//            ui->Mapwidget->replot();
//        }
//        else if(linestg == 0)
//        {
//            ssssline->setVisible(false);
//        }

    }
}
void MainWindow::on_mouseRealseEvent(QMouseEvent *event)
{
    //qDebug()<<"鼠标松下事件";
    // 获取鼠标点击的坐标（相对于界面）
   QPoint mousePressPos = event->pos();

   // 将界面坐标转换为图表坐标
   double xValue = ui->Mapwidget->xAxis->pixelToCoord(mousePressPos.x());
   double yValue = ui->Mapwidget->yAxis->pixelToCoord(mousePressPos.y());
    if(cusormode == normal && ifpress == 1)
    {
        QList<QCPAbstractItem*> selectedItem = ui->Mapwidget->selectedItems();
        foreach(QCPAbstractItem* item, selectedItem) {
        QCPItemText* textItem = qobject_cast<QCPItemText*>(item);
        if(textItem){
        QString name = textItem->text();
        qDebug()<<"正在移动中。。。"<<name<<xValue;
        json.removeLocation(name);
        json.addLocation("LocationdMark",name,xValue,yValue);
        ifpress = 0;
        }
        }
        PaintMap();

    }
}

void MainWindow::on_itemClick(QCPAbstractItem *item, QMouseEvent *event)
{
    qDebug() << "Item clicked!";
    if (item) {
            qDebug() << "Item clicked!";
            // 对于 QCPItemText 或者其他 QCPAbstractItem 的子类，你可以进一步处理特定的操作
            // 例如，输出项的坐标
            if (QCPItemText *textItem = dynamic_cast<QCPItemText*>(item)) {
                qDebug() << "Text item clicked at coordinates:" << textItem->position->coords();
            }
    }
}

/*
 通信功能的实现
    1、在主界面点击新建AMR设备，根据AMR不同的类型，使用不用的函数连接，连接成功后，获取连接成功的信息
    将连接成功的AMR的信息全部更新到listwiget上
    2、lsitwidget可以点击出现菜单，选择不同的槽函数，实现断开连接，或者其他不同的功能

*/

//void MainWindow::connectTcp(QString ip, QString AMRname, int AMRType)
//{
//    qDebug()<<"ip"<<ip<<AMRname<<AMRType;
//    int num = ui->AMRListWidget->count();
//    if(AMRType == AMR_xg)
//    {
//        //connect_thread(num,amr[num]);
//        // 创建自定义的 QListWidgetItem
//        QListWidgetItem *item = new QListWidgetItem(ip);
//        ui->AMRListWidget->addItem(item);
//    }


//}

void MainWindow::ConnectIP(QString ip, int type)
{
    if(type == 2){
        emit start_listen();
        qDebug()<<"监听成功；"<<endl;
    } else if(type == 0){ //仙工顶升机器人
         emit amr[I_AMR_B1]->ConnectSend(ip);
        amr[I_AMR_B1]->online_status = 1;
    } else if(type == 1){

    }
}

void MainWindow::update()
{

}

void MainWindow::UpdateMes()
{
    ui->Name_Edit->setText(json.MapName);
    ui->Version_Edit->setText(json.version);
    QString maxpos = QString("(%1,%2)").arg(json.MapPos.x()).arg(json.MapPos.y());
    ui->MaxPos_Edit->setText(maxpos);
    QString minpos = QString("(%1,%2)").arg(json.MinPos.x()).arg(json.MinPos.y());
    ui->MinPos_Edit->setText(minpos);
    QString str = QString("%1 条").arg(json.paths.size());
    ui->Line_Edit->setText(str);
    QString str2 = QString("%1 个").arg(json.Site_vector.size());
    ui->Point_Edit->setText(str2);
}

//void MainWindow::getCarData(double x,double y,double angle)
//{
//    int num = ui->AMRListWidget->count();
//    for(int i = 0; i< num ; i++)
//    {
//    AMR[i]->setItemPos(amr[i]->pos[0],amr[i]->pos[1]);
//    AMR[i]->setItemRotation(amr[i]->pos[2]);
//    ui->Mapwidget->replot();
//    }
//}

void MainWindow::PaintMap()  //绘制2D地图的主函数
{
    json.getAll();
    UpdateMes();
    ui->Mapwidget->clearGraphs();
    ui->Mapwidget->clearItems();
    ui->listWidget->clear();
    Dijistra_clear();
    On_num = 0;
    QPen mypen;
    mypen.setWidth(1);
    mypen.setColor(Qt::black);
    curGraph = ui->Mapwidget->addGraph();
    ui->Mapwidget->graph(0)->setPen(mypen);
    ui->Mapwidget->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->Mapwidget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssSquare, 0.4));
    //ui->Mapwidget->graph(0)->addData(x,y);
    ui->Mapwidget->graph(0)->setAdaptiveSampling(true);
    ui->Mapwidget->graph(0)->setSelectable(QCP::stMultipleDataRanges);
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::min();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();
    maxX = 5;
    maxY = 5;
    minX = -2;
    minY = -2;

    //创建障碍物
    // 添加点到图形对象中
    for (const QPointF& point : json.Barrier_vector) {

        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());

        ui->Mapwidget->graph(0)->addData(point.x(), point.y());
    }
    qDebug()<<"最大最小位置："<<minX<<maxX<<minY<<maxY;

    //站点构建遍历
    for (const auto& site : json.Site_vector) {
    QString siteName = site.Site_Name; // 获取站点名称
    // 遍历站点的坐标并添加到 QCustomPlot 中



        double x = site.start.x(); // 获取 x 坐标
        double y = site.start.y(); // 获取 y 坐标

        QCPItemText *textLabel = new QCPItemText(ui->Mapwidget);
        textLabel->setBrush(QBrush(QColor(0, 188, 212))); // 背景颜色设置为 RGB (0, 188, 212)
        textLabel->setColor(QColor(255, 255, 255)); // 文字颜色设置为 RGB (255, 255, 255)，即白色
        textLabel->setPositionAlignment(Qt::AlignHCenter|Qt::AlignVCenter); // 文字居中
        textLabel->position->setType(QCPItemPosition::ptPlotCoords); // 位置类型为实际坐标单位
        textLabel->position->setCoords(x, y); // 设置文本框坐标为 map.Mymarks[i].x, map.Mymarks[i].y
        textLabel->setText(siteName); // 设置文本内容为站点名称
        textLabel->setFont(QFont("Microsoft YaHei", 8, QFont::Bold)); // 设置字体及大小为微软雅黑，粗体，大小为10
        textLabel->setPen(QPen(Qt::green, 1, Qt::DashDotDotLine)); // 边框颜色设置为绿色，线宽为1，虚线样式
        textLabel->setPadding(QMargins(6, 6, 6, 6)); // 文字距离边框距离为12像素
        textLabel->setSelectedPen(QPen(Qt::blue, 2)); // 选中时边框颜色为蓝色，线宽为2像素
        textLabel->setSelectedFont(QFont("Microsoft YaHei", 8, QFont::Bold)); // 选中时字体设置为微软雅黑，粗体，大小为10
        textLabel->setSelectedColor(QColor(255, 255, 255)); // 选中时文字颜色设置为白色
        textLabel->setSelectedBrush(QBrush(QColor(0, 200, 250))); // 选中时背景颜色设置为 RGB (0, 200, 250)
        textLabel->setSelectable(true); // 允许文本框被选中
        ui->listWidget->addItem(siteName);

        QPointF pos(x,y); //当前站点的坐标 dijistra算法的需要
        position.append(pos);
        QVector<int> vec;
        QVector<qreal> vec2;
        matrix.insert(position.size(), vec);
        weight.insert(position.size(), vec);
        angle.insert(position.size(), vec2);


        // 正则表达式匹配字母和数字

       QRegularExpression re("(\\D+)|(\\d+)");
       QRegularExpressionMatchIterator matchIterator = re.globalMatch(siteName);
       QString numbers;
       QString letters;
       while (matchIterator.hasNext()) {
       QRegularExpressionMatch match = matchIterator.next();
                QString captured = match.captured(); // 获取匹配的字符串
                bool isNumber = captured[0].isDigit(); // 检查第一个字符是否是数字
                if (isNumber) {
                    numbers += captured; // 添加到数字字符串中
                } else {
                    letters += captured; // 添加到字母字符串中
                }
                if(numbers.toInt()>On_num){
                    On_num = numbers.toInt(); On_SiteName = letters;}
        }

    }

    // 创建连线
    for (const Path& path : json.paths) {
        QCPItemLine *line = new QCPItemLine(ui->Mapwidget);
        line->setHead(QCPLineEnding::esLineArrow);
        line->setPen(QPen(Qt::blue, 1, Qt::SolidLine)); //路线样式
        line->start->setCoords(path.start); // 设置起点坐标
        line->end->setCoords(path.end); // 设置终点坐标
        line->setPen(QPen(Qt::black, 1)); // 设置线条颜色和粗细
        QString name = path.Start_Name + "-" + path.End_Name;
        ui->listWidget->addItem(name);

        int deltaX = path.end.x() - path.start.x();
        int deltaY = path.end.y() - path.start.y();
        double agle = std::atan2(deltaY, deltaX);
        int start = 0,end = 0;
        for(int j = 0;j<position.size();j++)
        {
             if(position[j].x() == path.start.x() && position[j].y() == path.start.y())
             {
                 start = j;
                 break;
             }
        }
        for(int j = 0;j<position.size();j++)
        {
             if(position[j].x() == path.end.x() && position[j].y() == path.end.y())
             {
                 end = j;
                 break;
             }
        }
        matrix[start].append(end);
        angle[start].append(agle); //增加边以及角度信息
        weight[start].append(1);
    }




    // 设置坐标范围，增加一些边距以便站点不会贴在边缘
    double margin = 1.0;
    ui->Mapwidget->xAxis->setRange(minX - margin, maxX + margin);
    ui->Mapwidget->yAxis->setRange(minY - margin, maxY + margin);
    ui->Mapwidget->replot(); // 重新绘制

}

void MainWindow::handleTimeout(int i)
{
    QDateTime time = QDateTime::currentDateTime();
    QString time_s = time.toString("hh:mm:ss.zzz");
    QString msg = QString("pose %1 %2 %3 %4").arg(json.Site_vector[DijkstraPath[i]].start.x()).arg(json.Site_vector[DijkstraPath[i]].start.y()).arg(json.Site_vector[DijkstraPath[i]].angle/57.3).arg(time_s);
    QByteArray str= msg.toUtf8();
    qDebug()<<"发送的指令0427"<<msg<<"toUtf8:"<<str;
    emit send_tcp(str,1);
    qDebug()<<"成功发送指令"<<str;
}

void MainWindow::on_listWidget_itemPressed(QListWidgetItem *item)
{
    QMenu *pMenu = new QMenu;
    QAction *Nav = new QAction("导航至该站点", pMenu);
    QAction *del = new QAction("删除", pMenu);
    pMenu->addAction(Nav);
    pMenu->addAction(del);


    connect(Nav,&QAction::triggered,this,[=](){

       int des = ui->listWidget->row(item);
       DijkstraManager.setX(CurrentIndex);
       DijkstraManager.setY(des);
       qDebug()<<"导航成功"<<item->text()<<des;
       Dijistra_path();
       for(int i = DijkstraPath.size() - 2 ; i >= 0; i--){
           qDebug()<<"路径："<<DijkstraPath[i]<<json.Site_vector[DijkstraPath[i]].Site_Name;
           // 在你的类中，可能是一个窗口或者其他QObject派生类
           // 使用Lambda表达式来调用handleTimeout并传递参数
           int index = i;
           qDebug()<<"index:"<<index;
              QTimer::singleShot(2000 * (DijkstraPath.size() - 1 - i), this, [this, index]() {
                  this->handleTimeout(index);
              });
       }

    });

    connect(del,&QAction::triggered,this,[=](){

       if(cusormode != NONE)
       {
       QString instance = item->text();
       if (instance.contains("-")) {
          json.removeLine(instance);
       } else {
          json.removeLocation(instance);
       }
        qDebug()<<"删除成功"<<item->text();
        PaintMap();
       }
       else
       {
           // 显示警告对话框
           QMessageBox::warning(this, "警告", "请打开编辑按钮");
       }
    });

    //在鼠标位置显示菜单
    pMenu->exec(QCursor::pos());
    delete pMenu;
}



void MainWindow::on_AMRListWidget_itemPressed(QListWidgetItem *item)
{
    QMenu *pMenu = new QMenu;
    QAction *enable = new QAction("使能该AMR", pMenu);
    pMenu->addAction(enable);
    connect(enable,&QAction::triggered,this,[=](){
       qDebug()<<"断连成功"<<item->text();
       QString ip = item->text();
       int index = ui->AMRListWidget->row(item);
       //emit amr[index]->DisconnectSend(index);
    });

    //在鼠标位置显示菜单
    pMenu->exec(QCursor::pos());
    delete pMenu;

}


/*
   算法规划  计划将写出  单AMR算法，多AMR路径规划算法  以及区域冲突避免算法等
   1、单AMR路径规划算法  由于单AMR路径规划算法比较简单 可以多设计出几种不同的算法 以供后期对比 目前已经设计出dijistra算法
   2、多AMR路径规划算法  计划先将每辆AMR路径规划出来，然后对重合的路径加以重新规划
   3、区域冲突避免       对有可能造成冲突的区域 将实行仙工的 “冲突区规则” ，每个冲突区域只能存在一辆AMR
*/

void MainWindow::Dijistra_path()   //单AMR路径规划算法
{
    qDebug()<<"程序运行"<<endl;
    DijkstraPath.clear();
    int numOfVer = position.size();
    int edge[numOfVer][numOfVer];  //邻接矩阵
    int parent[numOfVer][numOfVer]; //parent[i][j]表示从结点i到结点j的路径上，j的前驱节点为结点parent[i][j]
    for(int i = 0; i < numOfVer; i++) {
        for(int j = 0; j < numOfVer; j++) {
            edge[i][j] = inf;
            parent[i][j] = i;
        }
    }

    for(int i = 0; i < numOfVer; i++){
        if(position[i].x() == erased)
            continue;
        for(int j = 0; j < matrix[i].size(); j++) {
            if(position[matrix[i][j]].x() == erased)
                continue;
            edge[i][matrix[i][j]] = weight[i][j];

        }
    }


    for(int i = 0; i < numOfVer; i++) {
        for(int j = 0; j < numOfVer; j++) {
            for(int k = 0; k < numOfVer; k++) {
                if(edge[j][k] > edge[j][i] + edge[i][k]){
                    edge[j][k] = edge[j][i] + edge[i][k];
                    parent[j][k] = parent[i][k];
                }
            }

        }

    }
    qDebug()<<"其中带你信息"<<DijkstraManager.x()<<DijkstraManager.y()<<edge[DijkstraManager.x()][DijkstraManager.y()];
    if(edge[DijkstraManager.x()][DijkstraManager.y()] != inf) {
        int i = DijkstraManager.y();
        while(i != DijkstraManager.x()) {
            DijkstraPath.append(i);
            i = parent[DijkstraManager.x()][i];
        }
        DijkstraPath.append(DijkstraManager.x());
    } else {
        DijkstraPath.append(inf);
    }
}



void MainWindow::Dijistra_clear()
{
    position.clear();     //清理站点坐标位置
    matrix.clear();
    weight.clear();
    angle.clear();
    DijkstraPath.clear();
    DijkstraManager.setX(isNotActive);
    DijkstraManager.setY(isNotActive);
}


/*
   数据库文件管理
   数据库起着保存数据的作用，包含任务数据、日志数据、报警数据、账户信息、机器人数据
   1、将新建的任务保存至数据库，以供执行时调用
   2、将使用过程中产生的文件保存至数据库，在界面中显示出来
   3、将任务数据以图表的形式展示。
*/
void MainWindow::openTable()
{
    DB = QSqlDatabase::addDatabase("QSQLITE"); //添加 SQL LITE数据库驱动
    DB.setDatabaseName("TangsxSQL.db3"); //设置数据库名称

    if (!DB.open())   //打开数据库
    {
        QMessageBox::warning(this, "错误", "打开数据库失败", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    TaskSQLModel=new CustomSqlQueryModel(this);

    //sqlmodel 任务信息表格
    TaskSQLModel->setQuery("SELECT ID, Type, Priority, TaskState, Start, End, Time,Source FROM Task where TaskState != '已完成' order by Priority");
    //时段控制数据库模型
    TaskSQLModel->setHeaderData(0,Qt::Horizontal,"任务ID");
    TaskSQLModel->setHeaderData(1,Qt::Horizontal,"任务类型");
    TaskSQLModel->setHeaderData(2,Qt::Horizontal,"优先级");
    TaskSQLModel->setHeaderData(3,Qt::Horizontal,"状态");
    TaskSQLModel->setHeaderData(4,Qt::Horizontal,"起点");
    TaskSQLModel->setHeaderData(5,Qt::Horizontal,"终点");
    TaskSQLModel->setHeaderData(6,Qt::Horizontal,"时间");
    TaskSQLModel->setHeaderData(7,Qt::Horizontal,"备注");
    ui->TaskTable->setModel(TaskSQLModel);
    theSelection=new QItemSelectionModel(TaskSQLModel);

}

void MainWindow::addMainBarrier(double x, double y)
{
    json.addBarrier(x,y);
}

void MainWindow::inital()
{
    amr[I_AMR_B1] = new car();
}


void MainWindow::getCarData(double x,double y,double angle){

}

void MainWindow::connected_success(QString ip){

}

/*
  为保障任务顺利执行，将原先的一套逻辑搬出来
  1、新建任务
  2、循环任务执行
  3、任务状态追踪检测
  4、删除任务
*/

void MainWindow::AcceptTaskinfo(int type, int priority, QString start, int startHigh, QString end, int endHigh, QString source,QPointF startP,QPointF endP)
{
    //判断任务起点终点是否相同，相同则返回
    if(start == end ){
        QMessageBox::information(this, "新建错误", "起点终点位置不可相同！",
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }


    QSqlQuery query;
    query.exec("select * from Task where ID =-1"); //实际不查询出记录，只查询字段信息
    //获取新建任务的具体信息
    QSqlRecord  recData;
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDate = currentDateTime.toString("yyyyMMdd");
    QString currentDate2 = currentDateTime.toString("yyyy-MM-dd");
    QString currentTime = currentDateTime.toString("hh:mm:ss");

    //获取数据库记录条数 以便后续编号
    int num = 0;
    QSqlQuery query1;
    query1.exec("select ID from Task");
    query1.last();
    QSqlRecord recodey = query1.record();
    QString value1 = recodey.value("ID").toString();
    if(value1 != NULL)
    {
    QStringList id = value1.split("-",QString::SkipEmptyParts);
    num = id[id.size() - 1].toInt();
    qDebug()<<"1133"<<num;
    }

    QString ID,tasktype;
    if(type == 0)
    {
        ID = QString("A-%1-%2").arg(currentDate).arg(num+1);
        tasktype = "移库";
    }
    else if(type == 1)
    {
        ID = QString("B-%1-%2").arg(currentDate).arg(num+1);
        tasktype = "出库";
    }
    else
    {
        ID = QString("C-%1-%2").arg(currentDate).arg(num+1);
        tasktype = "入库";
    }

    QString Prioritytask;
    if(priority == 0)
    {
       Prioritytask = "普通";
    }
    else
    {
       Prioritytask = "优先";
    }

    //新建query对象以实现插入记录操作
    query.prepare("INSERT INTO Task (ID, Type, TaskState, Time, Start,StartShelves, End, EndShelves, StartPointx, StartPointy, EndPointx, EndPointy,TaskPercent,Date,Priority,Source) "
                  "VALUES(:ID, :Type, :TaskState, :Time, :Start,:StartShelves, :End, :EndShelves, :StartPointx, :StartPointy, :EndPointx, :EndPointy, :TaskPercent,:Date, :Priority, :Source)");
    //绑定对应的值
    query.bindValue(":ID",ID);
    query.bindValue(":Type",tasktype);
    query.bindValue(":TaskState","待执行");
    query.bindValue(":Time",currentTime);
    query.bindValue(":Start",start);
    query.bindValue(":StartShelves",startHigh);
    query.bindValue(":End",end);
    query.bindValue(":EndShelves",endHigh);
    query.bindValue(":StartPointx",startP.x());
    query.bindValue(":StartPointy",startP.y());
    query.bindValue(":EndPointx",endP.x());
    query.bindValue(":EndPointy",endP.y());
    query.bindValue(":TaskPercent","-%0-");
    query.bindValue(":Date",currentDate2);  //Priority
    query.bindValue(":Priority",Prioritytask);
    query.bindValue(":Source",source);

    if (!query.exec())
        QMessageBox::critical(this, "错误", "插入记录错误\n"+query.lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
    else //插入，删除记录后需要重新设置SQL语句查询
    {
        QMessageBox::information(this, "新建成功", "新建任务成功！",
                                 QMessageBox::Ok,QMessageBox::NoButton);
        //此处要重新执行查询语句，不然显示的最大行数会被限制
        QString sqlStr=TaskSQLModel->query().executedQuery();//  执行过的SELECT语句
        TaskSQLModel->setQuery(sqlStr);         //重新查询数据
    }

    qDebug()<<"任务新建成功"<<type<<priority<<start<<startHigh<<end<<endHigh<<source<<startP<<endP;
}

void MainWindow::on_Delete_Button_clicked()   //4、删除任务
{
    if(QMessageBox::question(this,"确认删除","确认要删除？",QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
    {
        int curRecNo=theSelection->currentIndex().row();
        QSqlRecord  curRec=TaskSQLModel->record(curRecNo); //获取当前记录
        if (curRec.isEmpty()) //当前为空记录
            return;

        QString empNo=curRec.value("ID").toString();//获取员工编号
        QSqlQuery query;
        query.prepare("delete from Task where ID = :ID");
        query.bindValue(":ID",empNo);
        qDebug()<<"2121"<<empNo;

        if (!query.exec())
            QMessageBox::critical(this, "错误", "删除记录出现错误\n"+query.lastError().text(),
                                     QMessageBox::Ok,QMessageBox::NoButton);
        else //插入，删除记录后需要重新设置SQL语句查询
        {
            TaskSQLModel->query().exec();//数据模型重新查询数据，更新tableView显示
        }
    }
}
/*
    使用算法规划路径-》将规划好的路径放入数据库-》下发任务-》循环检测，并发下一段-》任务结束，改变数据库中的状态
*/
void MainWindow::on_TaskStart_Button_clicked() //点击开始按钮，首先需要规划路径
{
    TaskModel =new CustomSqlQueryModel(this);
    TaskModel->setQuery("SELECT ID, Type, Priority, TaskState, Start, End, Time,Source FROM Task where TaskState != '已完成' order by Priority");
    if (TaskModel->lastError().isValid())
    {
        QMessageBox::information(this, "错误", "数据表查询错误,错误信息\n"+TaskModel->lastError().text(),
                                 QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }
    QSqlRecord conData = TaskModel->record(CurrentSQL); //获取当前记录
    QString tk_type  = conData.value("Type").toString();
}



void MainWindow::on_PointAngle_edit_textChanged(const QString &arg1)
{

}
void MainWindow::on_SaveButton_clicked()
{
    double anglecur = ui->PointAngle_edit->text().toDouble();
    double str1 = ui->PointX_edit->text().toDouble();
    double str2 = ui->PointY_edit->text().toDouble();
    QString AMRname = ui->PointName_Edit->text();
    json.removeLocation(AMRname);
    json.addLocation("LocationdMark",AMRname,str1,str2,anglecur);
    PaintMap();
}

void MainWindow::on_pushButton_clicked()
{
    json.addBarrier(1,1);
}
