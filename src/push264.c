#include "push264.h"
#include "common.h"

//mk_pusher g_pusher = NULL; 
mk_media  g_media = NULL;

int init_push(void) {
    // 1. 初始化ZLMediaKit环境
    mk_config config = {
        .ini = NULL,
        .ini_is_path = 0,
        .log_level = 1,   // 0: Trace, 1: Debug, 2: Info
        .log_mask = LOG_CONSOLE,
        .ssl_is_path = 0,
        .ssl_pwd = NULL,
        .thread_num = 0
    };
    mk_env_init(&config);
    
    // 2. 启动 RTSP 服务器端口，默认 554
    mk_rtsp_server_start(554, 0);

    // 3. 创建媒体源 (MediaSource)
    // 播放地址将是: rtsp://<IP>:554/live/test
    g_media = mk_media_create("__defaultVhost__", "live", "test", 0, 0, 0);
    if (!g_media) {
        printf("Failed to create media source via mk_media_create\n");
        return -1;
    }

    // 4. 添加视频轨道 (H264)
    // codec_id: 0 (H264), width, height, fps, bit_rate: 0(auto)
    mk_media_init_video(g_media, 0, WIDTH, HEIGHT, FPS, 0);
    mk_media_init_complete(g_media);

    printf("ZLMediaKit RTSP Server started.\n");
    printf("Play URL: rtsp://<IP>:554/live/test\n");
    return 0;
}
