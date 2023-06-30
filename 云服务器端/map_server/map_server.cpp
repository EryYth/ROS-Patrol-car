#include <arpa/inet.h>
#include "ThreadPool.h"
#include <thread>
#include <fstream>
// #include <queue>
// #include <mutex>
// bool start = false;

typedef struct 
{
    int fd;
    bool open = true;
}fd_state,  *p_fd_state;

typedef struct 
{
    bool flag = true;
    int robotfd = 0;
    int appfd = 0;
}SRfd,  *p_SRfd;

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

void RobotToApp_fun(p_SRfd pfd)
{
    send_buff buff;
    int ret;
    unsigned int map_size;
    char* map_buff;
    int buff_size = sizeof(buff);
    int buff_temp;


    // while(!start);

    ret = write(pfd->robotfd, &"ok", 2);
    if(ret <= 0){
        perror("write");
        pfd->flag = false;
        return;
    }
// cout << "ret = "<< ret << endl;
    buff_temp = 4;
    while(buff_temp)
    {
        ret = recv(pfd->robotfd, ((unsigned char *)&map_size) + 4 - buff_temp, buff_temp, MSG_WAITALL);
        if(ret > 0){
            buff_temp = buff_temp - ret;
        }else{
            perror("recv");
            pfd->flag = false;
            return;
        }
    }
    ret = write(pfd->appfd, &map_size, 4);
    if(ret <= 0){
            perror("write");
        pfd->flag = false;
        return;
    }
    //  cout << "map_size = "<< map_size << endl;
    buff_temp = map_size;
    map_buff = new char[map_size];
    while(buff_temp)
    {
        ret = recv(pfd->robotfd, (char *)map_buff + map_size - buff_temp, buff_temp, MSG_WAITALL);
        if(ret > 0){
            buff_temp = buff_temp - ret;
        }else{
            perror("recv");
            pfd->flag = false;
            return;
        }
    }
    // fstream ifs;
    // ifs.open("./abc.pgm", ios::out | ios::binary);
    // ifs.write(map_buff, map_size);
    // ifs.close();
    // cout << "RobotToApp_fun this stop......." << endl;
    // while (1);
    ret = write(pfd->appfd, map_buff, map_size);
    // cout << "ret = "<< ret << endl;
    if(ret <= 0){
        perror("write");
        pfd->flag = false;
        return;
    }
    delete[] map_buff;
    // int i = 0;
    while(pfd->flag){
        buff_temp = buff_size;
        while(buff_temp)
        {
// cout << "::::::::::::::-" << i++ << "-::::::::::::::" << endl;
            ret = recv(pfd->robotfd, ((unsigned char *)&buff) + buff_size - buff_temp, buff_temp, MSG_WAITALL);
             if(ret > 0){
                buff_temp = buff_temp - ret;
            }else{
                perror("recv");
                pfd->flag = false;
                break;
            }
        }
        // cout << "angle = " << buff.angle << endl;
        // cout << "proportion_x = " << buff.proportion_x << endl;
        // cout << "proportion_y = " << buff.proportion_y << endl;
        ret = write(pfd->appfd, &buff, buff_size);
        if(ret <= 0){
            perror("write");
            pfd->flag = false;
            break;
        }
    }
}

void AppToRobot_fun(p_SRfd pfd)
{
    recv_buff buff;
    int ret;
    int buff_size = sizeof(buff);
    int buff_temp;

    ret = write(pfd->appfd, &"ok", 2);
    if(ret <= 0){
        perror("write");
        pfd->flag = false;
    }
    
    // start = true;
// cout << "AppToRobot_fun this stop......." << endl;
// while (1);
    while(pfd->flag){
        buff_temp = buff_size;
        while(buff_temp)
        {
            ret = recv(pfd->appfd, ((unsigned char *)&buff) + buff_size - buff_temp, buff_temp, MSG_WAITALL);
            if(ret > 0){
                buff_temp = buff_temp - ret;
            }else{
                perror("recv");
                pfd->flag = false;
                break;
            }
        }
        // cout << "buff.stop: " << buff.stop << endl;
        // cout << "buff.angle: " << buff.angle << endl;
        // cout << "buff.rotate: " << buff.rotate << endl;
        // cout << "buff.proportion_x: " << buff.proportion_x << endl;
        // cout << "buff.proportion_y: " << buff.proportion_y << endl;
        ret = write(pfd->robotfd, &buff, buff_size);
        if(ret <= 0){
            perror("write");
            pfd->flag = false;
            break;
        }
    }
}

int main(int argc, char *argv[])
{

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        perror("socket");
        exit(0);
    }

// 2. 将socket()返回值和本地的IP端口绑定到一起
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1000);   // 大端端口
    // INADDR_ANY代表本机的所有IP, 假设有三个网卡就有三个IP地址
    // 这个宏可以代表任意一个IP地址
    // 这个宏一般用于本地的绑定操作
    addr.sin_addr.s_addr = INADDR_ANY;  // 这个宏的值为0 == 0.0.0.0
//    inet_pton(AF_INET, "192.168.237.131", &addr.sin_addr.s_addr);
    int ret = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    // 3. 设置监听
    ret = listen(socket_fd, 128);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    // 4. 阻塞等待并接受客户端连接
    struct sockaddr_in cliaddr;
    char ip[24] = {0};
    char SR[4];
    int temp;
    SRfd srfd;
    unsigned int clilen = sizeof(cliaddr);
    for(int i = 0; i < 2; i++){
        // cout<<"等待连接。。。。。。。。。。。。。。。。。。"<<endl;
        temp=3;
        memset(SR, 0, sizeof(SR));
        memset(&cliaddr, 0, sizeof(cliaddr));
        int accept_fd = accept(socket_fd, (struct sockaddr*)&cliaddr, &clilen);
        cout << "accept = " << accept_fd << endl;
        if(accept_fd == -1)
        {
            perror("accept");
            exit(0);
        }
        usleep(3);
// cout<<"有连接。。。。。。。。。。。。。。。。。。"<<endl;
        while(temp)
        {
            ret = read(accept_fd, SR+3-temp, 3);
            cout<<"ret = "<< ret <<endl;
            if(ret <= 0){
                srfd.flag = false;
                close(accept_fd);
                return -1;
            }
            temp -= ret;
        }
        
// cout<<"SR = "<< SR <<endl;
        
        
        if(strcmp(SR,"APP")){
            srfd.robotfd = accept_fd;
            // 打印客户端的地址信息
            printf("小车端以连接 IP地址: %s, 端口: %d\n",
            inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
            ntohs(cliaddr.sin_port));
        }else{
            srfd.appfd = accept_fd;
            // 打印客户端的地址信息
            printf("手机端以连接 IP地址: %s, 端口: %d\n",
            inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
            ntohs(cliaddr.sin_port));
        }   
    }
    
    
    if(!srfd.flag){
        close(srfd.appfd);
        close(srfd.robotfd);
        return -1;
    }



    thread RobotToApp_thread(RobotToApp_fun, &srfd);
    thread AppToRobot_thread(AppToRobot_fun, &srfd);

    RobotToApp_thread.join();
    AppToRobot_thread.join();

    close(srfd.appfd);
    close(srfd.robotfd);

    return 0;
}