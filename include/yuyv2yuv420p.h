#ifndef _YUYV2YUV420P_H_
#define _YUYV2YUV420P_H_

#include "common.h"
#include <linux/types.h>
#include <stdio.h>

void yuyv2yuv420p(const uint8_t* yuyv_buf, uint8_t* yuv420p_buf);

#endif
