#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_Login_Button_clicked();

private:
    Ui::LoginPage *ui;
    bool m_bPressed; // 标记鼠标左键是否按下
    QPoint m_point; // 记录鼠标按下时相对于窗口的位置
};

#endif // LOGINPAGE_H
