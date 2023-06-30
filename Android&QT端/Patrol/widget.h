#ifndef WIDGET_H
#define WIDGET_H

#include "map.h"
#include "img.h"

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QIcon>



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    map* Map_Thread;
    img* Img_Thread;
    QPoint map_pix_size;                    //地图长宽
    QPoint map_pos_point;                   //地图位置
    QPixmap navigation_pix;                 //导航图标
    QPixmap navigation_matrix_pix;          //旋转后导航图标
    QPixmap target_pix;                     //目标图标
    QPoint target_deviation;                //目标图标偏移
    QRect target_rect;                      //目标图标大小
    QPoint navigation_pos_point;            //导航图标
    QPoint navigation_deviation;            //导航图标偏移
    bool Go_flag = true;

signals:
    void map_send_signal();
    void img_send_signal();

public slots:
    void map_slot();
    void img_slot();
    void navigation_point_slot();
    void arrive_slot();

private:
    Ui::Widget *ui;
    QRect rect;
    QPoint m_startMovePos;
protected:
    bool m_isPressed = false;//是否鼠标按下的标识
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_Go_pushButton_clicked();
    void on_right_pushButton_released();
    void on_right_pushButton_pressed();
    void on_left_pushButton_released();
    void on_left_pushButton_pressed();
    void on_pushButton_clicked();
};
#endif // WIDGET_H
