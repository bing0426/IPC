#include "get_yuv.h"
#include <stdio.h>
#include <linux/types.h>
#include "yuyv2yuv420p.h"
#include "yuv420p2h264.h"
#include "common.h"
#include "push264.h"

int main(int argc, char** argv){

    static int ret;
    uint8_t* yuv420p_buf;
    ret = get_yuv_init();
    if(ret < 0){
        fprintf(stderr,"%s\n", "get_yuv_init failed");
        return 0;
    }

    x264_encoder_init();

    
    ret = init_push();
    if(ret < 0){
        fprintf(stderr, "init push failed \n");
        return -1;
    }

    //通过VIDIOC_DQBUF获取v4l2_buffer v_buf 得到一帧数据

    //for(int i = 0; i < 150; ++i){
    while(1){
        ret = get_one_yuyv();
        if(ret < 0){
            fprintf(stderr,"%s\n", "get_one_yuyv failed");
            x264_encoder_deinit();
            return 0;
        }
        
        yuv420p_buf = malloc((int)(1.5 * IMG_SIZE));
        yuyv2yuv420p(mmap_addr[ret], yuv420p_buf);

        ret = return_buf();
        if(ret < 0)
            break;

        x264_encoder(yuv420p_buf);
        //printf("----%d----\n",i);
    }
    
    printf("----over----\n");

    encode_flush();

    x264_encoder_deinit();

    return 0;

}
