#ifndef CLASSFILE_H
#define CLASSFILE_H
#include "qcustomplot.h"
class CustomTextItem : public QCPItemText
{
public:
    CustomTextItem(QCustomPlot *parentPlot, double x, double y, double rotation, const QString &text)
        : QCPItemText(parentPlot)
    {
        // 设置文字属性
        setBrush(QBrush(QColor(131, 255, 235, 128))); // 设置背景颜色为半透明
        setColor(QColor(255, 255, 255)); // 设置文字颜色为白色
        setPositionAlignment(Qt::AlignHCenter | Qt::AlignVCenter); // 设置文字对齐方式
        setText(text); // 设置文字内容
        setFont(QFont("Microsoft YaHei", 10, QFont::Bold)); // 设置字体

        // 设置位置和角度
        position->setType(QCPItemPosition::ptPlotCoords); // 设置位置类型为绘图坐标
        position->setCoords(x, y); // 设置位置坐标
        setRotation(rotation); // 设置旋转角度

        // 设置边框颜色和宽度
        setPen(QPen(Qt::green, 2, Qt::DashDotDotLine)); // 设置边框颜色和宽度

        // 设置文字与边框的距离
        setPadding(QMargins(6, 6, 6, 6)); // 设置边距

        // 设置是否可选择
        setSelectable(false); // 设置不可选择
    }

    void setItemPos(double x,double y)
    {
       position->setCoords(x, y); // 设置位置坐标
    }
    void setItemRotation(int angle)
    {
        setRotation(angle); // 设置旋转角度
    }
    void setItemText(QString text)
    {
        setText(text); // 设置文字内容
    }

};







#include <QSqlQueryModel>

/*******************************************************
 *
 *
*******************************************************/
class CustomSqlQueryModel : public QSqlQueryModel
{
    public:
       CustomSqlQueryModel(QObject *parent = 0);

// QAbstractItemModel interface
    public:
       QVariant data(const QModelIndex &index, int role) const override;
};




#endif // CLASSFILE_H
