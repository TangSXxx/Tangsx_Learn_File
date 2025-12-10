#ifndef TABWIDGETCON_H
#define TABWIDGETCON_H

#include <vector>
#include <QWidget>
#include <QHBoxLayout>
#include <QTableWidget>

//目前该控件支持以下子控件
enum{
    BoxError = -1,
    CheckTWid = 0,  //CheckBox
    ComTWid ,       //combobox
    LineTWid        //LineEit
};

class TabWidgetCon :  public QWidget
{
    Q_OBJECT

public:
    explicit TabWidgetCon(std::vector<std::string> Headers, std::vector<int> HeadersType, std::vector<std::string> qItems, QWidget *parent = nullptr);
    explicit TabWidgetCon(QWidget *parent = nullptr);
    ~TabWidgetCon();

    //初始化UI界面
    void initUI();

    int GetColumnNum() { return mainTable->rowCount(); }
    int GetRowNum() { return mainTable->columnCount(); }
    QWidget* GetCellWidget(int row, int column) { return mainTable->cellWidget(row,column); };

    void setHeaders(std::vector<std::string> Headers, std::vector<int> HeadersType);
    void setqItems(std::vector<std::string> qItems);


public slots:
    void addLine();
    void deleteLine(int row);
    void setHeader(const std::vector<std::string> &headers);

private:
    QHBoxLayout * mainLayout  = nullptr;
    QTableWidget* mainTable   = nullptr;
    QStringList   m_headers;
    QStringList   m_Items;
    int* m_headersType = nullptr;
    int ItemSize = 0;


signals:
    void TableTextChanged(int row , int column , const QString& data);
    void TableItemChanged(int row , int column , const int& index,const QString& text);
    void TableStauChanged(int row , int column ,bool status);



};

#endif // TABWIDGETCON_H
