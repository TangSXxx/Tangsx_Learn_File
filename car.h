/*************************************************
* 定义仙工车类，新增车创建实体，后续主程序尽量采用对象形式
*************************************************/
#ifndef CAR_H
#define CAR_H
#include <QObject>
#include <QString>

enum {x = 0, y, angle};
//仙工车导航状态
enum {none = 0, waiting, running, suspended, completed, failed, canceled};
enum {offline=0,online};


class car : public QObject
{
    Q_OBJECT
private:
    //接收状态的属性
    //电量部分
    double battery_level;   //电池电量

    volatile int task_status;    //导航状态
    //发送指令设置的属性

    double ctrl_vx;     //遥控直线方向速度  m/s
    double ctrl_w;      //遥控旋转角速度   rad/s
    double ctrl_vz;     //遥控升举速度
    volatile double pos[3];  //小车位置
    QString ip;

public:
    bool online_status;
    bool control_status;    //是否持有控制权
    double max_speed;   //最大速度
    double max_wspeed;  //最大角速度
    double max_acc;     //最大加速度
    double max_wacc;    //最大角加速度
    volatile bool bot_status;   //新车的机械状态位
    volatile bool bot_warning;   //新车的报警状态位
    QString current_map;        //当前加载的地图名称

    friend class RecvThread;
    friend class MainWindow;

signals:
    void ConnectSend(QString ip);   //建立连接

    void DisconnectSend(int num);          //取消连接

    void send_task_list_sig(QStringList list);          //给定一组站点

    void send_ctrl_motion(double vx, double w = 0, double time = 0);            //开始开环运动

    void send_config_par(double speed, double wspeed, double acc, double wacc); //设置配置参数

    void send_ctrl_stop();  //停止开环运动

    void car_pausedown();   //暂停导航

    void car_resume();      //继续导航

    void car_cancel();      //取消导航

    void send_car_task(int pot, int mode, int frame_floor = 100, int car_floor = 100);

    void send_car_pot(QString point);

    void send_car_roll(int pot, int mode);

    void send_car_reset();

    void send_car_control_get();

    void send_car_control_release();

    void send_car_clean_errors();

    void send_car_get_currentmap();

};


#endif // CAR_H
