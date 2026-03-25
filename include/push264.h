#ifndef _PUSH264_H_
#define _PUSH264_H_

#include "linux/stddef.h"
#include "linux/types.h"
#include "mk_mediakit.h"
#include <stdint.h>
#include <stdio.h>

#define URL   "192.168.137.12:554"

//extern    mk_pusher g_pusher;
extern    mk_media  g_media;
int init_push(void);

#endif