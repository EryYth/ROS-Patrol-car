#include <arpa/inet.h>
#include "ThreadPool.h"
#include <thread>

typedef struct 
{
    bool flag = true;
    int robotfd = 0;
    int appfd = 0;
}SRfd,  *p_SRfd;

typedef struct 
{
    unsigned char obtain;
}recv_buff, *p_recv_buff;

typedef struct 
{
    unsigned int img_size = 0;
    char* img_buff;
}send_buff, *p_send_buff;

void RobotToApp_fun(p_SRfd pfd)
{
    send_buff buff;
    int ret;
    int buff_temp;
    cout<<"-----------------2----------------"<<endl;

    while(pfd->flag){
        cout<<"-----------------4----------------"<<endl;
        if(buff.img_size){
            buff_temp = buff.img_size;
            buff.img_buff = new char[buff.img_size];
            while(buff_temp)
            {
                ret = recv(pfd->robotfd, buff.img_buff + buff.img_size - buff_temp, buff_temp, MSG_WAITALL);
                if(ret > 0){
                    buff_temp = buff_temp - ret;
                }else{
                    perror("recv");
                    pfd->flag = false;
                    break;
                }
            }
            ret = write(pfd->appfd, buff.img_buff, buff.img_size);
            if(ret <= 0){
                perror("write");
                pfd->flag = false;
            }
            buff.img_size = 0;
            delete [] buff.img_buff;
        }else{
            buff_temp = 4;
            while(buff_temp)
            {
                ret = recv(pfd->robotfd, ((unsigned char *)&buff.img_size) + 4 - buff_temp, buff_temp, MSG_WAITALL);
                if(ret > 0){
                    buff_temp = buff_temp - ret;
                }else{
                    perror("recv");
                    pfd->flag = false;
                    break;
                }
            }
            ret = write(pfd->appfd, (char*)&buff.img_size, 4);
            if(ret <= 0){
                perror("write");
                pfd->flag = false;
            }
            cout << "img_size = " << buff.img_size << endl;
        }
    }
}

void AppToRobot_fun(p_SRfd pfd)
{
    recv_buff buff;
    int ret;

    while(pfd->flag){
        ret = recv(pfd->appfd, (unsigned char *)&buff.obtain, 1, MSG_WAITALL);
        if(ret <= 0){
            perror("recv");
            pfd->flag = false;
            break;
        }
        cout << "buff.obtain =" << buff.obtain << endl;
        ret = write(pfd->robotfd, (unsigned char *)&buff.obtain, 1);
        if(ret <= 0){
            perror("write");
            pfd->flag = false;
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
    addr.sin_port = htons(1001);   // 大端端口
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