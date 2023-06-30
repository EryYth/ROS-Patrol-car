#ifndef MAP_H
#define MAP_H
#include "top.h"

#define RADCHANGE (180 / 3.1415)

#define LEFT  1
#define RIGHT 2
#define OFF   3

typedef struct
{
    int stop,rotate;
    double proportion_x,proportion_y;
    double angle;
}map_recv_buff, *p_map_recv_buff;

typedef struct
{
    double proportion_x,proportion_y;
    double angle;
}map_send_buff, *p_map_send_buff;


class map: public QThread
{
    Q_OBJECT
public:

    explicit map(QHostAddress ip,QString port,QObject *parent = nullptr);
    ~map();

    map_recv_buff rbuff;        //小车目标点
    map_send_buff sbuff;        //收到小车位姿
    qint8 rbuff_size = sizeof(rbuff);
    qint8 sbuff_size = sizeof(sbuff);
    char* map_buff;         //接收地图数据数组
    quint32 map_size = 0;   //接收数组大小
    qint8 map_data = true;  //是否为地图数据
    QPixmap map_pix;        //地图
    volatile double angle,x,y;
    qint8 ok = false;
    QHostAddress ip;
    qint64 port;
    QTcpSocket *TcpSocket; //套接字

public slots:
    void Send_slot();
    void Receive_slot();

signals:
    void navigation_point_signal();
    void map_signal();
    void arrive_signal();

protected:
    void run();

};

#endif // MAP_H
