#include "tabwidgetcon.h"
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QCheckBox>
#include <vector>
#include <QDebug>
TabWidgetCon::TabWidgetCon(std::vector<std::string> Headers,std::vector<int> HeadersType,  std::vector<std::string> qItems , QWidget *parent)
    : QWidget(parent)
{
    //解析列头字符
    for (const auto &str : Headers) {
        m_headers << QString::fromStdString(str);
    }
    ItemSize = Headers.size();
    m_headersType = new int[ItemSize];
    for(int i = 0; i < ItemSize; ++i){
        if(i < HeadersType.size()) m_headersType[i] = HeadersType[i];
        else m_headersType[i] = -1;
    }

    //解析combox的item
    for (const auto &str : qItems) {
        m_Items << QString::fromStdString(str);
    }

    initUI();
}

TabWidgetCon::TabWidgetCon(QWidget *parent)
    : QWidget(parent)
{

}

TabWidgetCon::~TabWidgetCon()
{
    delete m_headersType;
}

void TabWidgetCon::initUI()
{
    mainLayout = new QHBoxLayout(this);

    mainTable = new QTableWidget(this);
    mainTable->setAlternatingRowColors(true);
    mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainTable->setSelectionMode(QAbstractItemView::SingleSelection);

    //设置表头内容及参数
    mainTable->setColumnCount(m_headers.size());
    mainTable->setHorizontalHeaderLabels(m_headers);

    mainTable->setStyleSheet(R"(
        /* QTableWidget 主表格样式 - 精简版 */
        QTableWidget {
            background-color: white;
            border: 1px solid #C0D8DA;
            border-radius: 6px;
            gridline-color: #E8F4F5;
            outline: 0;
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
            font-size: 14px;
            color: #2C3E50;
            selection-background-color: transparent;
            margin: 0px;
            padding: 0px;
        }

         /* 隐藏水平表头 */
         QHeaderView::section:vertical {
             width: 0px;
             padding: 0px;
             margin: 0px;
             border: none;
         }

        /* 表头样式 - 减少内边距 */
        QHeaderView::section {
            background-color: white;
            color: black;
            border: none;
            border-top: 1px solid #7AC8CC;
            border-bottom: 1px solid #7AC8CC;
            padding: 6px 8px;  /* 减少内边距 */
            font-weight: 600;
            font-size: 14px;
            margin: 0px;
        }

        /* 表格项样式 - 去掉内边距 */
        QTableWidget::item {
            border-bottom: 1px solid #F0F8F8;
            border-color:rgb(90,194,198);
            padding: 0px;  /* 去掉内边距 */
            margin: 0px;
            background-color: white;
        }

        /* 交替行颜色 */
        QTableWidget::item:alternate {
            background-color: #FAFDFD;
        }

        /* 选中项样式 */
        QTableWidget::item:selected {
            background-color: rgba(90,194,198, 0.15);
            color: #2C3E50;
            border: 1px solid rgb(90,194,198);
        }

        /* 鼠标悬停效果 */
        QTableWidget::item:hover {
            background-color: #F0F8F8;
        }

        /* 表格中的输入框样式 - 精简 */
        QTableWidget QLineEdit {
            border: 1px solid #C0D8DA;
            border-color:white;
            border-radius: 3px;
            padding: 2px 5px;  /* 减少内边距 */
            background-color: white;
            margin: 1px;
        }

        /* 表格中的组合框样式 - 精简 */
        QTableWidget QComboBox {
            color:black;
            border: 1px solid #C0D8DA;
            border-color:white;
            border-radius: 3px;
            padding: 2px 5px;  /* 减少内边距 */
            margin: 1px;
            min-width: 80px;
        }

        QTableWidget QComboBox::drop-down {
            width: 16px;
            border-left: 1px solid #C0D8DA;
        }

        /* 复选框样式 */
        QTableWidget QCheckBox {
            spacing: 2px;
            margin: 0px;
            padding: 0px;
        }

        QTableWidget QCheckBox::indicator {
            width: 14px;
            height: 14px;
        }

        /* 滚动条样式 */
        QScrollBar:vertical {
            background-color: #F8FBFB;
            width: 10px;
            margin: 0px;
            border: none;
        }

        QScrollBar::handle:vertical {
            background-color: #C0D8DA;
            min-height: 20px;
            border-radius: 5px;
        }

        QScrollBar:horizontal {
            background-color: #F8FBFB;
            height: 10px;
            margin: 0px;
            border: none;
        }

        QScrollBar::handle:horizontal {
            background-color: #C0D8DA;
            min-width: 20px;
            border-radius: 5px;
        }
    )");
    // 额外的表格美化设置
    mainTable->setAlternatingRowColors(true); // 启用交替行颜色
    mainTable->setFocusPolicy(Qt::NoFocus); // 移除焦点框，使用CSS样式
    mainTable->horizontalHeader();

    mainLayout->addWidget(mainTable);
}

void TabWidgetCon::setHeaders(std::vector<std::string> Headers, std::vector<int> HeadersType)
{
    for (const auto &str : Headers) {
        m_headers << QString::fromStdString(str);
    }
    ItemSize = Headers.size();
    m_headersType = new int[ItemSize];
    for(int i = 0; i < ItemSize; ++i){
        if(i < HeadersType.size()) m_headersType[i] = HeadersType[i];
        else m_headersType[i] = -1;
    }
}

void TabWidgetCon::setqItems(std::vector<std::string> qItems)
{
    //解析combox的item
    for (const auto &str : qItems) {
        m_Items << QString::fromStdString(str);
    }
}

void TabWidgetCon::addLine()
{
    if(mainTable == nullptr) qDebug() << "akxbaklc";
    int row = mainTable->rowCount();
    mainTable->insertRow(row);

    for(int i = 0; i < m_headers.size(); ++i){
        if(m_headersType[i] == ComTWid){
            QComboBox *comboBox = new QComboBox();
            comboBox->addItems(m_Items);
            mainTable->setCellWidget(row, i, comboBox);
            connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                    [this , i , comboBox](int index) {

                if(!comboBox) return;

               int currentRow = -1;
               for (int r = 0; r < mainTable->rowCount(); ++r) {
                   if (mainTable->cellWidget(r, i) == comboBox) {
                       currentRow = r;
                       break;
                   }
               }
               if(currentRow != -1)
               emit TableItemChanged(currentRow, i, index,comboBox->currentText());
            });
        }else if(m_headersType[i] == CheckTWid){
            //确保该控件位于方格中间
            QWidget *checkBoxContainer = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(checkBoxContainer); // 将布局设置到容器
            QCheckBox* CBox  = new QCheckBox(this);

            layout->addWidget(CBox);
            layout->setAlignment(Qt::AlignCenter); // 核心：设置布局内控件居中对齐
            layout->setContentsMargins(0, 0, 0, 0);
            mainTable->setCellWidget(row, i, checkBoxContainer);

            connect(CBox , &QCheckBox::stateChanged , [this , i , CBox](int state){
                if(CBox == nullptr) {
                    qDebug() << "cbox is nullptr";
                    return;
                }

                int currentRow = -1;
                for (int r = 0; r < mainTable->rowCount(); ++r) {
                    QWidget *cellWidget = mainTable->cellWidget(r, i);
                    if (cellWidget) {
                        // 如果单元格里是容器，查找里面的 QCheckBox
                        QCheckBox *box = cellWidget->findChild<QCheckBox*>();
                        if (box == CBox) {
                            currentRow = r;
                            break;
                        }
                    }
                }

                if(currentRow != -1) emit TableStauChanged(currentRow , i , state);
            });
        }else if(m_headersType[i] == LineTWid){
            QLineEdit *lineEdit = new QLineEdit();
            lineEdit->setPlaceholderText(QString("请输入%1").arg(m_headers[i]));
            mainTable->setCellWidget(row, i, lineEdit);
            connect(lineEdit , &QLineEdit::textChanged , [this , i , lineEdit](QString edText){
                if(!lineEdit) return;

                int currentRow = -1;
                for (int r = 0; r < mainTable->rowCount(); ++r) {
                    if (mainTable->cellWidget(r, i) == lineEdit) {
                        currentRow = r;
                        break;
                    }
                }

                if(currentRow != -1)
                emit TableTextChanged(currentRow , i , edText);
            });
        }
        mainTable->resizeColumnToContents(i);
        mainTable->resizeRowsToContents();
    }
}

void TabWidgetCon::deleteLine(int row)
{
    if (row >= 0 && row < mainTable->rowCount()) {
        mainTable->removeRow(row);

        for (int i = row; i < mainTable->rowCount(); ++i) {
            QWidget *widget = mainTable->cellWidget(i, mainTable->columnCount() - 1);
            if (widget) {
                QPushButton *deleteBtn = widget->findChild<QPushButton*>();
                if (deleteBtn) {
                    deleteBtn->setProperty("row", i);
                }
            }
        }
    }
}

void TabWidgetCon::setHeader(const std::vector<std::string> &headers)
{

}
