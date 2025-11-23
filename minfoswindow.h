#ifndef MINFOSWINDOW_H
#define MINFOSWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "src/fostcpserver.h"
#include <string>
using namespace std;
namespace Ui {
class MinFOSWindow;
}

class MinFOSWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MinFOSWindow* instance();

    static void WriteMes(const QString& content);

    ~MinFOSWindow();

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_ComListWidget_itemClicked(QListWidgetItem *item);

    void on_StartTCPserver_Button_clicked();

    void on_StopTCPserver_Button_clicked();

private:
    explicit MinFOSWindow(QWidget *parent = nullptr);
    Ui::MinFOSWindow *ui;
};

#endif // MINFOSWINDOW_H
