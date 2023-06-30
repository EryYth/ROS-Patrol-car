#include "img.h"

img::img(QHostAddress ip, QString port, QObject *parent):  QThread(parent)
{
    this->ip = ip;
    this->port = port.toInt();
    this->TcpSocket = new QTcpSocket(this);
    this->TcpSocket->connectToHost(this->ip, this->port);
    memset(&this->rbuff, 0, this->rbuff_size);
    this->TcpSocket->write("APP");
//    qDebug() << "已发送";
    connect(TcpSocket, &QTcpSocket::readyRead, this, &img::Receive_slot);
}

img::~img()
{
    if(this->img_buff != nullptr)delete[] this->img_buff;
    this->img_buff = nullptr;
}

void img::Send_slot()
{
    rbuff.obtain = 1;
    this->TcpSocket->write((char *)&rbuff, this->rbuff_size);
    rbuff.obtain = 0;
    qDebug() << "已发送 size = " << this->rbuff_size;
}

void img::Receive_slot()
{
    if(this->img_size){     //地图
        if(this->TcpSocket->bytesAvailable()< this->img_size)return;
        if(this->img_buff != nullptr)delete[] this->img_buff;
        this->img_buff = new char[this->img_size];
        this->TcpSocket->read(this->img_buff, this->img_size);
        img_pix.loadFromData((uchar *)this->img_buff, this->img_size,"JPG");
        this->img_size = 0;
        emit img_signal();
    }else{                  //大小
        if(this->TcpSocket->bytesAvailable()< 4)return;
        this->TcpSocket->read((char*)&this->img_size, 4);
    }
}
