#include "get_yuv.h"
#include "common.h"
#include "linux/perf_event.h"


    static  int fd, ret;
    int i = 0;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    uint8_t* mmap_addr[4] = {0};
    int   mmap_lenght[4];
    FILE *fp = NULL;
    struct v4l2_fmtdesc v_fmtdesc;
    struct v4l2_capability v_capability;
    struct v4l2_format v_fmt;
    struct v4l2_frmsizeenum v_frmsize;
    struct v4l2_frmivalenum v_frnival;
    struct v4l2_requestbuffers v_reqbuf;
    struct v4l2_streamparm v_sparm;
    struct v4l2_buffer v_buf;

int open_dev(void){ 
    fd = open("/dev/video9", O_RDWR);
    if(fd < 0){
        perror("open:");
        return -1;
    }
    printf("open succeed fd: %d\n",fd);
    memset(&v_fmtdesc, 0, sizeof(v_fmtdesc));
    v_fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while(1){
        v_fmtdesc.index = i++;
        //通过ioctl去获取摄像头支持的视频格式
        ret = ioctl(fd, VIDIOC_ENUM_FMT, &v_fmtdesc);
        if(ret < 0 ){
            if(errno == EINVAL){
                printf("---------over-----------\n");
                break;
            }
            else{
                perror("ioctl ENUM_FMT");
                close(fd);
            }
        }
        printf("index:%d    description:%s\n",v_fmtdesc.index, v_fmtdesc.description);
    }
    return 0;
}
    //获取设备支持那些功能
void get_capability(void){
    memset(&v_capability, 0, sizeof(v_capability));
    ret = ioctl(fd, VIDIOC_QUERYCAP, &v_capability);
    if(ret < 0){
        perror("ioctl QUERYCAP");
        close(fd);
    }
    else{
        printf("driver: %s  bus_info: %s\n",v_capability.driver, v_capability.bus_info);
        printf("capability: %#x\n",v_capability.capabilities);
    }
}
    //获取设备分辨率yuv
void get_frmsize(void){
    memset(&v_frmsize, 0, sizeof(v_frmsize));
    v_frmsize.index = 0;
    v_frmsize.pixel_format = V4L2_PIX_FMT_YUYV;
    while(1){
        ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &v_frmsize);
        if(ret < 0){
            if(errno == EINVAL)
                break;
            else{
                perror("ioctl VIDIOC_ENUM_FRAMESIZES");
                close(fd);
            }
        }
        if(v_frmsize.type ==  V4L2_FRMSIZE_TYPE_DISCRETE){
            printf("index: %d width: %d height: %d\n",v_frmsize.index, v_frmsize.discrete.width, v_frmsize.discrete.height);
        }
        ++(v_frmsize.index);
    }
}
    //查询在1920*1080分辨率下设备支持的帧率
void get_frnival(void){
    memset(&v_frnival, 0, sizeof(v_frnival));
    v_frnival.index = 0;
    v_frnival.width = WIDTH;
    v_frnival.height = HEIGHT;
    v_frnival.pixel_format = V4L2_PIX_FMT_YUYV;
    while(1){
        ret = ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &v_frnival);
        if(ret < 0){
            if(errno == EINVAL){
                break;
            }else{
                perror("ioctl VIDIOC_ENUM_FRAMEINTERVALS");
                close(fd);
            }
        }
        if(v_frnival.type == V4L2_FRMIVAL_TYPE_DISCRETE){
            if(v_frnival.discrete.numerator){
                printf("fps: %.3f\n", (double)v_frnival.discrete.denominator/v_frnival.discrete.numerator);
            }
        }
        ++(v_frnival.index);
    }
}
    //配置摄像头
void set_fmt(void){
    memset(&v_fmt, 0, sizeof(v_fmt));
    /*传输type,确定fmt union*/
    v_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v_fmt.fmt.pix.width = WIDTH;
    v_fmt.fmt.pix.height = HEIGHT;
    v_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    /*确定摄像头为逐行采集*/
    v_fmt.fmt.pix.field = V4L2_FIELD_NONE;
    ret = ioctl(fd, VIDIOC_S_FMT, &v_fmt);
    if(ret < 0){
        perror("ioctl VIDIOC_S_FMT");
        close(fd);
    }
    
    ret = ioctl(fd, VIDIOC_G_FMT, &v_fmt);
    if(ret < 0){
        perror("ioctl VIDIOC_G_FMT");
        close(fd);
    }
    printf("width: %d height: %d\n",v_fmt.fmt.pix.width, v_fmt.fmt.pix.height);
    if(v_fmt.fmt.pix.pixelformat & V4L2_BUF_TYPE_VIDEO_CAPTURE){
        printf("set V4L2_BUF_TYPE_VIDEO_CAPTURE succeed\n");
    }else{
        printf("set V4L2_BUF_TYPE_VIDEO_CAPTURE failed\n");
    }
    if (v_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
        printf("set pixelformat YUYV succeed\n");
    } else {
        printf("set pixelformat adjusted: %c%c%c%c\n",
               v_fmt.fmt.pix.pixelformat & 0xff,
               (v_fmt.fmt.pix.pixelformat >> 8) & 0xff,
               (v_fmt.fmt.pix.pixelformat >> 16) & 0xff,
               (v_fmt.fmt.pix.pixelformat >> 24) & 0xff);
    }
}
    /*配置摄像头帧率*/
void set_sparm(void){
    memset(&v_sparm, 0, sizeof(v_sparm));
    v_sparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v_sparm.parm.capture.timeperframe.numerator = 1;//时间为一秒
    v_sparm.parm.capture.timeperframe.denominator = FPS;//间隔
    ret = ioctl(fd, VIDIOC_S_PARM, &v_sparm);
    if(ret < 0){
        perror("ioctl VIDIOC_S_PARM");
        close(fd);
    }
}
    /*申请缓冲空间*/
void reqbuf(void){
    memset(&v_reqbuf, 0, sizeof(v_reqbuf));
    v_reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v_reqbuf.count = 4;
    v_reqbuf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_REQBUFS, &v_reqbuf);
    if(ret < 0){
        perror("ioctl VIDIOC_REQBUFS");
        close(fd);
    }
}
    /*查询申请的缓冲空间信息用于映射*/
void mmap_buf(void){
    memset(&v_buf, 0, sizeof(v_buf));
    v_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for(int i = 0;  i < 4; ++i){
        v_buf.index = i;
        ret = ioctl(fd, VIDIOC_QUERYBUF, &v_buf);
        if(ret < 0){
            perror("ioctl VIDIOC_QUERYBUF");
            close(fd);
        }
        mmap_addr[i] = mmap(0, v_buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, v_buf.m.offset);
        if(mmap_addr[i] == MAP_FAILED){
            perror("mmap");
            close(fd);
        }
        mmap_lenght[i] = v_buf.length;
        
        ret = ioctl(fd, VIDIOC_QBUF, &v_buf);
        if(ret < 0){
            perror("ioctl VIDIOC_DQBUF");
            close(fd);
        }
    }
}
    /*开流*/
void open_stream(void){
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if(ret < 0){
        perror("ioctl VIDIOC_STREAMON");
        close(fd);
    }
}
    /*保存yuyv
void save_yuyv(void){
    fp = fopen("/tmp/out.yuyv", "wb");
    if(!fp){
        perror("fopen");
        close(fd);
    }

    v_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v_buf.memory = V4L2_MEMORY_MMAP;
    for(int i = 0; i < 150; ++i){
        ret = ioctl(fd, VIDIOC_DQBUF, &v_buf);
        if(ret < 0){
            printf("Line:%d ",__LINE__);
            perror("ioctl VIDIOC_DQBUF");
            goto err;
        }
       ret = fwrite(mmap_addr[v_buf.index], 1, v_buf.bytesused, fp);
       if(ret != v_buf.bytesused){
            perror("fwrite");
            fclose(fp);
            goto err;
       }
       ioctl(fd, VIDIOC_QBUF, &v_buf);
       printf("已采集帧数：%d\n", i);
    }
    fclose(fp);
}
*/

int get_yuv_init(void){
    ret = open_dev();
    if(ret < 0)
        return -1;

    //get_capability();
    //get_frmsize();
    //get_frnival();
    set_fmt();
    set_sparm();
    reqbuf();
    mmap_buf();
    open_stream();
}

int get_one_yuyv(void){
    v_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v_buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_DQBUF, &v_buf);
    if(ret < 0){
        perror("get_one_yuyv");
        return -1;
    }
    printf("----get one yuyv succeed---\n");
    return ret;

}

int return_buf(void){
    ret = ioctl(fd, VIDIOC_QBUF, &v_buf);
    if(ret < 0){
        perror("return buf failed");
        return -1;
    }
    return 0;
}
