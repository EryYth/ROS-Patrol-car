#include "widget.h"
#include "ui_widget.h"

#include <QSlider>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    ui->num_Slider->setValue(0);
    QHostAddress local_ip("120.79.151.190");
    QString map_port = "1000";
    QString img_port = "1001";
    this->Map_Thread = new map(local_ip, map_port);
    this->Img_Thread = new img(local_ip, img_port);
//    this->map_pix = new QPixmap;

    this->navigation_pix = QPixmap(":/current_point.png");
    this->target_pix = QPixmap(":/Target_point.png");
    this->target_rect = ui->target_label->rect();
    this->navigation_deviation = QPoint(-ui->navigation_label->rect().center());
    ui->navigation_label->setScaledContents(true);
    ui->target_label->setScaledContents(true);
//    ui->navigation_label->setPixmap(this->navigation_pix);
    ui->target_label->setPixmap(this->target_pix);

    this->Map_Thread->start();
    connect(this->Map_Thread, &map::map_signal, this, &Widget::map_slot);
    connect(this->Img_Thread, &img::img_signal, this, &Widget::img_slot);
    connect(this->Map_Thread, &map::navigation_point_signal, this, &Widget::navigation_point_slot);
    connect(this, &Widget::map_send_signal, this->Map_Thread, &map::Send_slot);
    connect(this, &Widget::img_send_signal, this->Img_Thread, &img::Send_slot);
    connect(this->ui->num_Slider, &QSlider::valueChanged, this, [=](){      //获取角度信息
        this->Map_Thread->rbuff.angle = ui->num_Slider->value();
        ui->num_label->setNum(ui->num_Slider->value());
    });
    connect(this->Map_Thread, &map::arrive_signal, this, &Widget::arrive_slot);
//    this->Map_Thread->map_pix = QPixmap(":/SUSE_B7-5map.pgm");
//    emit this->Map_Thread->map_signal();

}

Widget::~Widget()
{
    delete ui;
    delete Map_Thread;
    delete Img_Thread;
}

void Widget::map_slot()
{
    this->Map_Thread->map_pix = this->Map_Thread->map_pix.scaled(1080,1120,Qt::KeepAspectRatio);
    ui->map_lable->setPixmap(this->Map_Thread->map_pix);
    //地图大小
    this->map_pix_size = QPoint(this->Map_Thread->map_pix.width(),this->Map_Thread->map_pix.height());
    //地图位置  label(左上角)+中心位置-地图大小的一半
    this->map_pos_point = QPoint(ui->map_lable->pos()+ui->map_lable->rect().center()-this->map_pix_size/2);
//    qDebug() << "map_pix_size：width =" << this->map_pix_size.rx()<< "height = " << this->map_pix_size.ry();
//    qDebug() << "map_pos_point：width =" << this->map_pos_point.rx() << "height = " << this->map_pos_point.ry();
    emit this->Map_Thread->TcpSocket->readyRead();
//    this->map_pix->loadFromData((uchar *)this->Map_Thread->map_buff, this->Map_Thread->map_size,"PGM");
//    *this->map_pix = this->map_pix->scaled(1080,1120,Qt::KeepAspectRatio);
//    ui->map_lable->setPixmap(*this->map_pix);
//    this->map_pix_size = QPoint(this->map_pix->width(),this->map_pix->height());
    //    this->map_pos_point = QPoint(ui->map_lable->pos()+ui->map_lable->rect().center()-this->map_pix_size/2);
}

void Widget::img_slot()
{
    QMatrix matrix;
    matrix.rotate(180);
    this->Img_Thread->img_pix = this->Img_Thread->img_pix.scaled(1080,1120,Qt::KeepAspectRatio);
    this->Img_Thread->img_pix = this->Img_Thread->img_pix.transformed(matrix);
    ui->image_label->setPixmap(this->Img_Thread->img_pix);
}

//小车的位姿信息
void Widget::navigation_point_slot()
{
//    qDebug() << "-----------------处理------------------";
    QMatrix matrix;
    matrix.rotate(this->Map_Thread->angle);                             //获取角度
    this->navigation_matrix_pix = this->navigation_pix.transformed(matrix);   //设置角度
    ui->navigation_label->setPixmap(this->navigation_matrix_pix);                   //修改后的坐标
    this->navigation_pos_point.setX(this->Map_Thread->x*this->map_pix_size.x());
    this->navigation_pos_point.setY((1-this->Map_Thread->y)*this->map_pix_size.y());
//    qDebug() << "----------slot angle = " << this->Map_Thread->angle;
//    qDebug() << "navigation_pos_point x = " << navigation_pos_point.rx() <<" y = " << navigation_pos_point.ry();
//    qDebug() << "map_pos_point x = " << map_pos_point.rx() <<" y = " << map_pos_point.ry();
//    qDebug() << "navigation_deviation x = " << navigation_deviation.rx() <<" y = " << navigation_deviation.ry();
    ui->navigation_label->move(navigation_pos_point+this->map_pos_point+this->navigation_deviation);

//    qDebug() << "-----------------3------------------";

}

void Widget::arrive_slot()
{
    this->Go_flag = true;
    ui->Go_pushButton->setStyleSheet("QPushButton{border-image: url(:/go.png)}");
    ui->left_pushButton->setEnabled(true);
    ui->right_pushButton->setEnabled(true);
    ui->num_Slider->setEnabled(true);

    this->Map_Thread->rbuff.stop = true;
    this->Map_Thread->rbuff.rotate = OFF;
    emit map_send_signal();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
//    if(event->pos().x() < this->rect.topLeft().x()  ||
//       event->pos().x() > this->rect.topRight().x() ||
//       event->pos().y() < this->rect.topLeft().y()  || (this->Go_Stop_icon == &this->stop_icon)
//       event->pos().y() > this->rect.bottomLeft().y())return;
    if((event->pos().x() > 880 && event->pos().y() > 1120)  ||
       (!this->Go_flag)    )return;
    this->m_isPressed = true;
    m_startMovePos = event->globalPos() - ui->target_label->frameGeometry().topLeft();

}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(this->m_isPressed == false)return;
    QPoint pos = event->pos() - m_startMovePos;
    int rx = pos.rx(),ry = pos.ry();
    if(rx<0)rx = 0;
    if(rx>1060)rx = 1060;
    if(ry<1140)ry = 1140;
    if(ry>2150)ry = 2180;
    QPoint temp(rx,ry);
    ui->target_label->move(temp);
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    this->m_isPressed = false;
    this->rect = ui->target_label->frameGeometry();

}



void Widget::on_Go_pushButton_clicked()
{
    if(this->Go_flag){                                              //点击开始
        this->Go_flag = false;
        ui->Go_pushButton->setStyleSheet("QPushButton{border-image: url(:/stop.png)}");
        ui->left_pushButton->setEnabled(false);
        ui->right_pushButton->setEnabled(false);
        ui->num_Slider->setEnabled(false);

        this->Map_Thread->rbuff.stop = false;
        this->Map_Thread->rbuff.rotate = 0;
        this->Map_Thread->rbuff.angle = ui->num_Slider->value();
        this->Map_Thread->rbuff.proportion_x = (ui->target_label->pos().x()+this->target_rect.center().x()-this->map_pos_point.x()) / (float)this->map_pix_size.x();
        this->Map_Thread->rbuff.proportion_y = 1 - (ui->target_label->pos().y()+this->target_rect.center().y()-this->map_pos_point.y()) / (float)this->map_pix_size.y();
        emit map_send_signal();
    }else {                                                         //点击停止
        this->Go_flag = true;
        ui->Go_pushButton->setStyleSheet("QPushButton{border-image: url(:/go.png)}");
        ui->left_pushButton->setEnabled(true);
        ui->right_pushButton->setEnabled(true);
        ui->num_Slider->setEnabled(true);

        this->Map_Thread->rbuff.stop = true;
        this->Map_Thread->rbuff.rotate = OFF;
        emit map_send_signal();

//        qDebug()<< " x:"<< this->Map_Thread->rbuff.proportion_x <<" y:"<< this->Map_Thread->rbuff.proportion_y<< " angle:"<<this->Map_Thread->rbuff.angle;

    }
}

void Widget::on_right_pushButton_released()     //释放
{
//    qDebug() << "-----------------3------------------";
    this->Map_Thread->rbuff.rotate = OFF;
    ui->right_pushButton->setStyleSheet("QPushButton{border-image: url(:/right.png)}");
    emit map_send_signal();
}

void Widget::on_right_pushButton_pressed()      //按下
{
//    qDebug() << "-----------------1------------------";
    this->Map_Thread->rbuff.rotate = RIGHT;
    ui->right_pushButton->setStyleSheet("QPushButton{border-image: url(:/right-R.png)}");
    emit map_send_signal();
}

void Widget::on_left_pushButton_released()      //释放
{
    this->Map_Thread->rbuff.rotate = OFF;
    ui->left_pushButton->setStyleSheet("QPushButton{border-image: url(:/left.png)}");
    emit map_send_signal();
}

void Widget::on_left_pushButton_pressed()       //按下
{
    this->Map_Thread->rbuff.rotate = LEFT;
    ui->left_pushButton->setStyleSheet("QPushButton{border-image: url(:/left-R.png)}");
    emit map_send_signal();
}

void Widget::on_pushButton_clicked()
{
    emit img_send_signal();
}
