#include "map.h"


map::map(QHostAddress ip, QString port, QObject *parent):  QThread(parent)
{
    this->ip = ip;
    this->port = port.toInt();
    this->TcpSocket = new QTcpSocket(this);
    this->TcpSocket->connectToHost(this->ip, this->port);
    memset(&this->rbuff, 0, this->rbuff_size);
    this->TcpSocket->write("APP");
//    qDebug() << "已发送";
    connect(TcpSocket, &QTcpSocket::readyRead, this, &map::Receive_slot);
}

map::~map()
{
    delete [] this->map_buff;
    delete (this->TcpSocket);
}

void map::Send_slot()
{
    if(!this->ok)return;
    this->TcpSocket->write((char *)&rbuff, this->rbuff_size);
//    qDebug() << "-----------------2------------------";
}

void map::run()
{
    this->exec();
}

void map::Receive_slot()
{
    if(!this->ok){
        if(this->TcpSocket->bytesAvailable()< 2)return;
        char OK[2];
        this->TcpSocket->read(OK, 2);
        if(OK[0]=='o' && OK[1]=='k')
            this->ok = 1;
    }
    if(this->map_data){     //接收地图数据
        if(this->map_size){     //地图
            if(this->TcpSocket->bytesAvailable()< this->map_size)return;
            this->map_buff = new char[this->map_size];
            this->TcpSocket->read(this->map_buff, this->map_size);
            map_pix.loadFromData((uchar *)this->map_buff, this->map_size,"PGM");
            this->map_data = false;
            emit map_signal();
        }else{                  //大小
            if(this->TcpSocket->bytesAvailable()< 4)return;
            this->TcpSocket->read((char*)&this->map_size, 4);
        }
    }else{                  //接收位置数据
        if(this->TcpSocket->bytesAvailable() < this->sbuff_size)return;
//        qDebug() << " ---  if return : " << this->TcpSocket->bytesAvailable();
        while(this->TcpSocket->bytesAvailable() / this->sbuff_size){
//        qDebug() << " ---      while : " << this->TcpSocket->bytesAvailable();
            this->TcpSocket->read((char*)&this->sbuff, this->sbuff_size);
        }
//        qDebug() << " ---after while : " << this->TcpSocket->bytesAvailable();
        this->angle = -this->sbuff.angle * RADCHANGE + 90;
        this->x = this->sbuff.proportion_x;
        this->y = this->sbuff.proportion_y;
        if(fabs((this->sbuff.proportion_x*this->sbuff.proportion_x + this->sbuff.proportion_y*this->sbuff.proportion_y) -
            (this->rbuff.proportion_x*this->rbuff.proportion_x + this->rbuff.proportion_y*this->rbuff.proportion_y))  <  0.001){



        }
//        qDebug() << "-----------------收到------------------";
//        qDebug() << "-------sbuff.angle = " << this->angle;
        qDebug() << "sbuff.proportion_x = " << this->sbuff.proportion_x;
        qDebug() << "sbuff.proportion_y = " << this->sbuff.proportion_y;
        emit navigation_point_signal();
//        qDebug() << "-----------------1------------------";
    }
}
