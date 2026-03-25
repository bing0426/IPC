#include "yuyv2yuv420p.h"



void yuyv2yuv420p(const uint8_t* yuyv_buf, uint8_t* yuv420p_buf){
    uint8_t* yuv420p_y = yuv420p_buf;
    uint8_t* yuv420p_u = yuv420p_buf + IMG_SIZE;
    uint8_t* yuv420p_v = yuv420p_buf + (int)(1.25 * IMG_SIZE);
    int num = 0;

    for(int i = 0; i < HEIGHT; ++i){
        const uint8_t* yuyv_line = yuyv_buf + i * 2 * WIDTH;

        for(int j = 0; j < WIDTH; j+=2){
            yuv420p_y[i*WIDTH+j] = yuyv_line[2*j];
            yuv420p_y[i*WIDTH+j+1] = yuyv_line[2*j+2];         
        }

        if(i % 2 == 0 && i+1 < HEIGHT ){
            for(int k = 0; k < WIDTH; k+=2){
                uint8_t u1 = yuyv_line[k*2+1];
                uint8_t v1 = yuyv_line[k*2+3];
                uint8_t u2 = yuyv_line[2 * WIDTH +k*4+1];
                uint8_t v2 = yuyv_line[2 * WIDTH +k*4+3];
                yuv420p_u[num] = (u1 + u2) /2;
                yuv420p_v[num] = (v1 + v2) /2;
                ++num;

            }
        }
    }
    printf("----yuyv to yuv420P succeed----\n");

}