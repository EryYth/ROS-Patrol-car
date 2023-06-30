#ifndef IMG_H
#define IMG_H

#include "top.h"

typedef struct
{
    unsigned char obtain;
}img_recv_buff, *p_img_recv_buff;

typedef struct
{
    unsigned int img_size;
    char* img_buff;
}img_send_buff, *p_img_send_buff;

class img: public QThread
{
    Q_OBJECT
public:
    explicit img(QHostAddress ip,QString port,QObject *parent = nullptr);
    ~img();

    QHostAddress ip;
    qint64 port;
    QTcpSocket *TcpSocket; //套接字

    img_recv_buff rbuff;
    img_send_buff sbuff;
    qint8 rbuff_size = sizeof(rbuff);
    qint8 sbuff_size = sizeof(sbuff);
    quint32 img_size = 0;
    char* img_buff = nullptr;
    QPixmap img_pix;

signals:
    void img_signal();

public slots:
    void Send_slot();
    void Receive_slot();
};

#endif // IMG_H
