#ifndef _GET_YUV_H_
#define _GET_YUV_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <linux/types.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include "common.h"

extern uint8_t* mmap_addr[4];

int get_yuv_init(void);
int get_one_yuyv(void);
int return_buf(void);

#endif
