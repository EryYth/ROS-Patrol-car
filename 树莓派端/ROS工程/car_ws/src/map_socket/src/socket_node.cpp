#include "ros/ros.h"
#include "actionlib_msgs/GoalID.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Pose.h"
#include "nav_msgs/MapMetaData.h"
#include <thread>
#include <mutex>
#include <fcntl.h>
#include <fstream>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <move_base_msgs/MoveBaseActionGoal.h>
#include <nav_msgs/OccupancyGrid.h>


volatile double height_pos,width_pos;                    //地图最大坐标
 double roll,picth,yaw;                          //横滚，俯仰，偏航
 double angle;                                   //角度
volatile double x,y;                                     //坐标
volatile double deviation_x,deviation_y,deviation_z;     //初始坐标
volatile bool map_flag = false;
volatile bool send_once = false;

using namespace std;

typedef struct 
{
    int fd;
    bool open = false;
}fd_state,  *p_fd_state;



typedef struct 
{
    int stop,rotate;
    double proportion_x,proportion_y;
    double angle;
}recv_buff, *p_recv_buff;

typedef struct 
{
    double proportion_x,proportion_y;
    double angle;
}send_buff, *p_send_buff;

// 通过stat结构体 获得文件大小，单位字节
size_t getFileSize1(const char *fileName) {

	if (fileName == NULL) {
		return 0;
	}
	
	// 这是一个存储文件(夹)信息的结构体，其中有文件大小和创建时间、访问时间、修改时间等
	struct stat statbuf;

	// 提供文件名字符串，获得文件属性结构体
	stat(fileName, &statbuf);
	
	// 获取文件大小
	size_t filesize = statbuf.st_size;

	return filesize;
}

void Get_Map_Data(const nav_msgs::OccupancyGrid::ConstPtr& map_data)
{
    //得到初始角度
    tf2::Quaternion q;  //四元素转换
    q.setX(map_data->info.origin.orientation.x);
    q.setY(map_data->info.origin.orientation.y);
    q.setZ(map_data->info.origin.orientation.z);
    q.setW(map_data->info.origin.orientation.w);
    tf2::Matrix3x3(q).getRPY(roll,picth,yaw);
    //得到初始坐标
    deviation_x = map_data->info.origin.position.x;
    deviation_y = map_data->info.origin.position.y;
    deviation_z = map_data->info.origin.position.z;
    //得到地图最大坐标
    height_pos = map_data->info.height / 20.0;
    width_pos = map_data->info.width / 20.0;
    map_flag = true;
}

void recv_fun(p_fd_state pfd)
{
    ros::NodeHandle nh;
    actionlib_msgs::GoalID goalID;
    move_base_msgs::MoveBaseActionGoal move_pos;
    geometry_msgs::Twist twist;
    tf2::Quaternion q;  //四元素转换

    //发布
    ros::Publisher pub_stop = nh.advertise<actionlib_msgs::GoalID>("/move_base/cancel",1);
    ros::Publisher pub_rotate = nh.advertise<geometry_msgs::Twist>("/cmd_vel",1);
    ros::Publisher pub_goal = nh.advertise<move_base_msgs::MoveBaseActionGoal>("/move_base/goal",1);
    //订阅
    ros::Subscriber sub_map = nh.subscribe<nav_msgs::OccupancyGrid>("/map",1,Get_Map_Data);

    int ret, temp;  //套接字接收用变量
    recv_buff buff; //接收信息
    int buff_size = sizeof(buff);   //接收信息的长度

    //初始化运动
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;
    twist.linear.x = 0;
    twist.linear.y = 0;
    twist.linear.z = 0;
    //初始化目标点

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1)
    {
        perror("socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1000);   // 大端端口
    inet_pton(AF_INET, "120.79.151.190", &addr.sin_addr.s_addr);

    ret = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }

    //*****************************************************
    // char buf[1024];
    // while(1){
    //     memset(buf, 0, 1024);
    //     read(socket_fd, buf, 1024);
    //     write(socket_fd, "hello server", sizeof("hello server"));
    // }
    //*****************************************************
    while (map_flag);

    ret = write(socket_fd, (char*)&"ROT", 3);

    pfd->fd = socket_fd;
    
    pfd->open = true;
    
    while(pfd->open){
        ret = buff_size;
        while(ret)
        {
            temp = recv(pfd->fd, ((unsigned char *)&buff) + buff_size - ret, ret, MSG_WAITALL);
            if(temp > 0){
                ret = ret - temp;
            }else{
                perror("recv");
                pfd->open = false;
                break;
            }
        }
        if(!pfd->open){pub_stop.publish(goalID);break;}
        //取消导航点
        if(buff.stop)pub_stop.publish(goalID);  
        //发布导航点
        move_pos.header.stamp = ros::Time::now();
        move_pos.goal.target_pose.header.stamp = ros::Time::now();
        move_pos.goal.target_pose.header.frame_id = "map";

        move_pos.goal.target_pose.pose.position.x = width_pos * buff.proportion_x - deviation_x;
        move_pos.goal.target_pose.pose.position.y = height_pos * buff.proportion_y - deviation_y;
        move_pos.goal.target_pose.pose.position.z = -deviation_z;
        q.setRPY(0 - roll, 0 - picth, buff.angle - yaw);
        move_pos.goal.target_pose.pose.orientation.x = q.getX();
        move_pos.goal.target_pose.pose.orientation.y = q.getY();
        move_pos.goal.target_pose.pose.orientation.z = q.getZ();
        move_pos.goal.target_pose.pose.orientation.w = q.getW();
        pub_goal.publish(move_pos);
        //发布运动信息
        switch(buff.rotate){
            case 0:break;
            case 1:{
                twist.linear.x = 0.05;
                twist.angular.z = 0.5;
                pub_rotate.publish(twist);
                break;}
            case 2:{
                twist.linear.x = 0.05;
                twist.angular.z = -0.5;
                pub_rotate.publish(twist);
                break;}
            case 3:{
                twist.angular.z = 0;
                twist.linear.x = 0;
                pub_rotate.publish(twist);
                break;}
            default:break;
        }
        ros::spinOnce();
        ros::spinOnce();
        ros::spinOnce();
        // ros::spinOnce();
        // ros::spinOnce();
        // ros::spinOnce();
    }
    close(socket_fd);
}

void Get_Amcl_Data(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& amcl_data)
{
    double temp;
    tf2::Quaternion q;  //四元素转换
    
    q.setX(amcl_data->pose.pose.orientation.x);
    q.setY(amcl_data->pose.pose.orientation.y);
    q.setZ(amcl_data->pose.pose.orientation.z);
    q.setW(amcl_data->pose.pose.orientation.w);
    tf2::Matrix3x3(q).getRPY(temp,temp,angle);
    angle += yaw;
    x = amcl_data->pose.pose.position.x + deviation_x;
    y = amcl_data->pose.pose.position.y + deviation_y;
    // ROS_INFO("-------------------------x = %.2f, y= %.2f----------------------------", x, y);
    send_once = true;
}

void send_fun(p_fd_state pfd)
{
    // int fd = open("~/arrow.png", O_RDONLY);
    char ch[] = "/home/yth/ROS/car_ws/src/nav/map/四川轻化工大学B7_5楼地图.pgm";
    fstream ofs;
    ofs.open(ch, ios::in | ios::binary);
    unsigned int fds = getFileSize1(ch);
    char *file_data = new char[fds] ;
    ofs.read(file_data, fds);
    ofs.close();
    // fstream ifs;
    // ifs.open("/home/yth/abc.pgm", ios::out | ios::binary);
    // ifs.write(file_data, fds);
    // ifs.close();
    int ret = 2,temp;
    char OK[2];

    while (!pfd->open);     //等待套接字连接

    while(ret){
	    ROS_INFO("this......");
        temp = recv(pfd->fd, ((unsigned char *)OK) + 2 - ret, ret, MSG_WAITALL);
    	// ROS_INFO("---------------------------------recv byte = %d --------------------------------", temp);
        if(temp > 0){
            ret = ret - temp;
        }else{
            perror("recv");
            pfd->open = false;
            break;
        }
    }
    if(!(OK[0] == 'o' && OK[1] == 'k')){
        pfd->open = false;
        return; 
    }
    //printf("fds = %d", fds);
    ret = write(pfd->fd, (char*)&fds, 4);
    if(ret <= 0){
        perror("write");
        pfd->open = false;
        return;
    }    
    ret = write(pfd->fd, file_data, fds);
    if(ret <= 0){
        perror("write");
        pfd->open = false;
        return;
    }
    
    ros::NodeHandle nh;
    ros::Subscriber sub_amcl = nh.subscribe<geometry_msgs::PoseWithCovarianceStamped>("/amcl_pose",1,Get_Amcl_Data);

    
    send_buff buff;
    int i = 0;
    while(pfd->open){
        while(!send_once)ros::spinOnce();
        ROS_INFO("----------------send_once +%d----------------", i++);
        buff.angle = angle;
        buff.proportion_x = x / width_pos;
        buff.proportion_y = y / height_pos;
        ret = write(pfd->fd, &buff, sizeof(buff));
        if(ret <= 0){
            perror("write");
            pfd->open = false;
            return;
        }
        send_once = false;
    }  
  
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    ros::init(argc, argv, "socket_node");
    fd_state fds;
    
    thread recv_thread(recv_fun, &fds);
    thread send_thread(send_fun, &fds);

    while(fds.open){

        ros::spinOnce();
    }

    recv_thread.join();
    send_thread.join();


    return 0;
}
