#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/mman.h>
#include <arpa/inet.h>

#define OBTAIN 1


int main(void) 
{
    unsigned char obtain;
    int fd = open("/dev/video0",O_RDWR);
    if (fd < 0)
    {
        perror("打开设备失败");
        return -1;
    }

    //获取摄像头支持格式 ioctl(文件描述符,命令，与命令对应的结构体)
    struct v4l2_format vfmt;

    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //摄像头采集
    vfmt.fmt.pix.width = 1400; //设置摄像头采集参数，不可以任意设置
    vfmt.fmt.pix.height = 1256;
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG; //设置为mjpg格式，则我可以直接写入文件保存，YUYV格式保存后需要转换格式才能查看
    
    int ret = ioctl(fd,VIDIOC_S_FMT,&vfmt);
    if (ret < 0)
    {
        perror("设置格式失败1");
    }

    //申请内核空间
    struct v4l2_requestbuffers reqbuffer;
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuffer.count = 1; //申请4个缓冲区
    reqbuffer.memory = V4L2_MEMORY_MMAP;  //映射方式

    ret = ioctl(fd,VIDIOC_REQBUFS,&reqbuffer);
    if (ret < 0)
    {
        perror("申请空间失败");
    }
   
    //映射
    unsigned char *mptr[1];//保存映射后用户空间的首地址
    unsigned int size[1];
    struct v4l2_buffer mapbuffer;
    //初始化type和index
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for(int i = 0; i <1;i++) {
        mapbuffer.index = i;
        ret = ioctl(fd,VIDIOC_QUERYBUF,&mapbuffer); //从内核空间中查询一个空间作映射
        if (ret < 0)
        {
            perror("查询内核空间失败");
        }
        //映射到用户空间
        mptr[i] = (unsigned char *)mmap(NULL,mapbuffer.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,mapbuffer.m.offset);
        size[i] = mapbuffer.length; //保存映射长度用于后期释放
    }

    /********************************************************************************/
    /************************************套接字***************************************/
    /********************************************************************************/
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1)
    {
        perror("socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1001);   // 大端端口
    inet_pton(AF_INET, "120.79.151.190", &addr.sin_addr.s_addr);

    ret = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }
    unsigned char connect_flag = 1;
    ret = write(socket_fd, (char*)&"ROT", 3);
    if(ret <= 0){
        perror("write");
        exit(0);
    }  
    /********************************************************************************/
    while(1){
        ret = recv(socket_fd, (unsigned char *)&obtain, 1, MSG_WAITALL);
        if(ret <= 0){
            perror("recv");
            break;
        }
        if(obtain == OBTAIN){
            printf("开始采集\n");
            /********************************************************************************/
            /************************************取照片***************************************/
            /********************************************************************************/
            //开始采集
            ret = ioctl(fd,VIDIOC_STREAMON,&type); 
            if (ret < 0)
            {
                perror("开启失败");
            }
            for(int i = 0; i < 4;i++){
                ret = ioctl(fd,VIDIOC_QBUF,&mapbuffer); 
                if (ret < 0)
                {
                    perror("放回失败");
                }

                ret = ioctl(fd,VIDIOC_DQBUF,&mapbuffer); 
                if (ret < 0)
                {
                    perror("读取数据失败");
                }
            }
            //停止采集
            ret = ioctl(fd,VIDIOC_STREAMOFF,&type);
            /********************************************************************************/

                // fwrite( mptr[readbuffer.index],readbuffer.length,1,file);//写入文件
            ret = write(socket_fd, (char*)&mapbuffer.bytesused, 4);
            if(ret <= 0){
                perror("write");
                break;
            } 
            printf("readbuffer->length = %d\n", mapbuffer.bytesused);
            ret = write(socket_fd, mptr[mapbuffer.index], mapbuffer.bytesused);
            if(ret <= 0){
                perror("write");
                break;
            }  
            obtain = 0;
        }

    }





    //通知内核使用完毕
   ret = ioctl(fd, VIDIOC_QBUF, &mapbuffer);
   if(ret < 0)
	{
		perror("放回队列失败");
	}
    //停止采集
    ret = ioctl(fd,VIDIOC_STREAMOFF,&type);
    
	//释放映射
    for(int i=0; i<1; i)munmap(mptr[i], size[i]);

    close(fd); //关闭文件
    close(socket_fd);
    return 0;
}
