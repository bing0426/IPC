#include "yuv420p2h264.h"
#include "linux/types.h"
#include "push264.h"


static int ret;
int iNal = 0;
int ipts = 0;
static int has_published = 0;
//static FILE* fd;
const int csp = X264_CSP_I420;
x264_nal_t* pNals   = NULL;
x264_t*     pHandle = NULL;
x264_picture_t* pPic_in = NULL;
x264_picture_t* pPic_out = NULL;
x264_param_t* pParam = NULL;

void x264_encoder_init(void){
    pPic_in = (x264_picture_t*)malloc(sizeof(x264_picture_t));
    pPic_out = (x264_picture_t*)malloc(sizeof(x264_picture_t));
    pParam = (x264_param_t*)malloc(sizeof(x264_param_t));
    x264_param_default_preset(pParam, "ultrafast", "zerolatency");
    pParam->i_width = WIDTH;
    pParam->i_height = HEIGHT;
    pParam->i_csp = csp;
    pParam->i_fps_den = 1;
    pParam->i_fps_num = FPS;
    pParam->i_timebase_den = 1;
    pParam->i_timebase_num = FPS;
    pParam->b_repeat_headers = 1;
    pParam->b_annexb = 1;
    x264_param_apply_profile(pParam, x264_profile_names[5]);

    pHandle = x264_encoder_open(pParam);

    x264_picture_init(pPic_out);
    x264_picture_alloc(pPic_in, csp, pParam->i_width, pParam->i_height);

    //fd = fopen("./test.h264", "wb");
}

int x264_encoder(uint8_t* yuv420p_buf){
        memcpy(pPic_in->img.plane[0], yuv420p_buf, IMG_SIZE);
        //printf("----Y over----\n");
        memcpy(pPic_in->img.plane[1], yuv420p_buf+IMG_SIZE, IMG_SIZE / 4);
        //printf("----U over----\n");
        memcpy(pPic_in->img.plane[2], yuv420p_buf + (int)(1.25 * IMG_SIZE), IMG_SIZE /4);
        //printf("----V over----\n");

        pPic_in->i_pts = ipts++;
        printf("---ipts:%d---\n",ipts);

        ret = x264_encoder_encode(pHandle, &pNals, &iNal, pPic_in, pPic_out);
        if(ret < 0){
            perror("x264_encoder_encode");
            return -1;
        }
    
    for (int j = 0; j < iNal; ++j) {
        printf("NAL[%d]: type=%d, size=%d\n", j, pNals[j].i_type, pNals[j].i_payload);
    }

    if(g_media && iNal >0){
        size_t total_size = 0;
        for(int i = 0; i < iNal; ++i){
            total_size += pNals[i].i_payload;
        }

        uint8_t* data = (uint8_t*)malloc(total_size);
        if(!data){
            printf("data malloc failed \n");
            return -1;
        }

        uint8_t* ptr = data;
        for(int i = 0; i  < iNal; ++i){
            memcpy(ptr, pNals[i].p_payload, pNals[i].i_payload);
            ptr += pNals[i].i_payload;
        }

        int is_key_frame = 0;
        for (int i = 0; i < iNal; ++i) {
            // NAL_SLICE_IDR=5, NAL_SPS=7
            if (pNals[i].i_type == NAL_SLICE_IDR || pNals[i].i_type == NAL_SPS) {
                is_key_frame = 1;
                break;
            }
        }

        uint32_t dts_ms = (pPic_out->i_dts * 1000) / FPS;
        uint32_t pts_ms = (pPic_out->i_pts * 1000) / FPS;

        mk_media_input_h264(g_media, data, total_size, dts_ms, pts_ms);

        if (is_key_frame && has_published == 0) {
            printf(">>>>>>>>>>>> Got First Key Frame, Stream is ready! <<<\n");
            has_published = 1;
        }

        free(data);

    }
    /*
    for (int j = 0; j < iNal; ++j){
        fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, fd);
    }
    */
}

void encode_flush(void){
    while(1){
        ret = x264_encoder_encode(pHandle, &pNals, &iNal, NULL, pPic_out);
        if (ret == 0) {
            break;
        }
        printf("Flush 1 frame.\n");
        printf("%d\n",has_published);
        /*for (int j = 0; j < iNal; ++j) {
            fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, fd);
        }*/
	}
}

void x264_encoder_deinit(void){

    x264_picture_clean(pPic_in);
    x264_encoder_close(pHandle);
    pHandle = NULL;

    free(pPic_in);
    free(pPic_out);
    free(pParam);
	//fclose(fd);
    
}