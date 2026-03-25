#ifndef _YUV420P2H264_H_
#define _YUV420P2H264_H_

#include <stdint.h>
#include <x264.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/types.h>
#include <errno.h>
#include "common.h"

extern x264_t*     pHandle;

void x264_encoder_init(void);
int x264_encoder(uint8_t* yuv420p_buf);
void x264_encoder_deinit(void);
void encode_flush(void);

#endif

