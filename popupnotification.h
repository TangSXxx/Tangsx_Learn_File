#ifndef POPUPNOTIFICATION_H
#define POPUPNOTIFICATION_H
#include <QMainWindow>

namespace Ui {
class popupNotification;
}

class popupNotification : public QMainWindow
{
    Q_OBJECT

public:
    explicit popupNotification(QWidget *parent = nullptr,QString ip = nullptr);
    ~popupNotification();




private slots:
    void on_Confirm_Button_clicked();

    void on_AMRType_Box_currentIndexChanged(int index);

    void on_Cancel_Button_clicked();

private:
    Ui::popupNotification *ui;

signals:
    void SendConnect(QString ip,int type);   //   type:0-仙工   1-自研


};

#endif // POPUPNOTIFICATION_H
