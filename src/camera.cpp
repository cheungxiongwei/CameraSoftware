//
// Created by cheungxiongwei on 2022/3/29.
//

#include "camera.h"
#include <QDebug>

//ffmpeg
#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"//编码/解码库
#include "libavdevice/avdevice.h"//特殊设备复合/复制库
#include "libavfilter/avfilter.h"//基于图形的框架编辑库
#include "libavformat/avformat.h"// I/O 格式库
#include "libavutil/avutil.h"//所有FFmpeg库共享的通用代码
#include "libpostproc/postprocess.h"//视频后处理库
#include "libswresample/swresample.h"//音频处理重采样，样本格式转换和混合的库
#include "libswscale/swscale.h"//颜色转换和缩放库
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif

static std::once_flag once_init;

class Camera_ {
public:
    Camera_(Camera* obj)
        : q(obj) {
        std::call_once(once_init, []() { avdevice_register_all(); });
    }
    Camera* q;

    //
    AVDictionary *dict_option = nullptr;//键值字典
    AVInputFormat *input_fmt = nullptr;//输入文件容器格式
    AVFormatContext *fmt_ctx = nullptr;//输入流上下文

    AVCodec *codec = nullptr;//解码器
    AVCodecContext *codec_ctx = nullptr;//解码器上下文

    AVPacket *packet = nullptr;//压缩数据包
    AVFrame *decode_freme = nullptr;//解码数据帧
    AVFrame *rgb_frame = nullptr;//转换数据帧
    std::uint8_t *rgb_buffer = nullptr;

    QImage image;//数据缓存区

    SwsContext *sws_ctx = nullptr;//格式转换上下文

    int got_picture = 0;
    int video_stream_idx = -1;//视频流索引下标
    int audio_stream_idx = -1;//音频流索引下标
    double input_fps = 0.0;

    bool is_run = false;
    bool is_exit = false;

    //重置参数
    inline void reset(){
        got_picture = 0;
        video_stream_idx = -1;
        audio_stream_idx = -1;
        input_fps = 0.0;
        is_run = false;
        is_exit = false;
    }

    inline void init_find_dshow(){//寻找 dshow 驱动输入格式
        input_fmt = av_find_input_format("dshow");//寻找指定驱动输入格式
    }

    //RTMP and RTSP protocol, time base: seconds;
    //HTTP protocol, time base: microseconds (not milliseconds).
    inline void init_rtsp_dict(){
        //设置参数
        av_dict_set(&dict_option,"rtbufsize","10M",0);
        av_dict_set(&dict_option,"rtsp_transport","tcp",0);
        //av_dict_set(&dict_option,"allowed_media_types","video",0);//设置仅接受视频数据 {"video","audio","data"}
        //av_dict_set(&dict_option,"timeout","5",0);//设置超时时间 5s

        //网络延时时间
        av_dict_set(&dict_option, "max_delay", "200", 0);
        av_dict_set(&dict_option, "stimeout", "3000000", 0);

    }
};
Camera::Camera(QObject* parent)
    : QObject(parent),d(std::make_unique<Camera_>(this)) {
    //d = std::make_unique<Camera_>(this);
}
Camera::~Camera() {
    if(!d->is_run){
        close();
    }
}
bool Camera::open(const QString& url) {
    qDebug() << url;
    if(d->is_run){
        close();
    }

    // step 1 设置参数
    if(url.startsWith("rtsp")){
        d->init_rtsp_dict();
    }else{
        //default
    }

    d->init_rtsp_dict();


    //设置回调中断函数
    //int interrupt_cb(void * ctx){
    //    return 0;
    //}
    //const AVIOInterruptCB int_cb = {[](void *ctx)->int{},NULL};
    //d->fmt_ctx->interrupt_callback = int_cb;

    // step 2
    if(avformat_open_input(&d->fmt_ctx,url.toStdString().data(),0,&d->dict_option) != 0){
        av_dict_free(&d->dict_option);
        qWarning("Couldn't open input stream.");
        return false;
    }
    av_dict_free(&d->dict_option);

    // step 3 find stream info
    if(avformat_find_stream_info(d->fmt_ctx,0) < 0){
        avformat_close_input(&d->fmt_ctx);
        qWarning("Couldn't find stream information.");
        return false;
    }

    // step 4 find video stream idx and audio stream idx
    for(unsigned int i = 0;i < d->fmt_ctx->nb_streams;++i){
        if(d->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            d->video_stream_idx = i;
            break;
        }
        if(d->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            d->audio_stream_idx = i;
            break;
        }
    }

    //判断是否找到视频流索引
    if(d->video_stream_idx == -1){
        avformat_close_input(&d->fmt_ctx);
        qWarning("Couldn't find stream video_stream_idx.");
        return false;
    }

#if 0
    double input_fps = static_cast<double>(d->fmt_ctx->streams[d->video_stream_idx]->r_frame_rate.num)
            / static_cast<double>(d->fmt_ctx->streams[d->video_stream_idx]->r_frame_rate.den);
    qDebug() << "FPS:" << input_fps;
#endif

    // step 5
    //获取当前流解码器参数
    d->codec_ctx = avcodec_alloc_context3(0);
    avcodec_parameters_to_context(d->codec_ctx,d->fmt_ctx->streams[d->video_stream_idx]->codecpar);
    d->codec = avcodec_find_decoder(d->codec_ctx->codec_id);

    if(d->codec == 0){
        avformat_close_input(&d->fmt_ctx);
        qWarning("Couldn't find the decoder.");
    }

    // step 6 open av codec
    //打开视频流解码器
    if(avcodec_open2(d->codec_ctx,d->codec,0) < 0){
        avformat_close_input(&d->fmt_ctx);
        qWarning("Couldn't to open the av codec.");
        return false;
    }

    // step 7
    // 设置格式转换上下文
    //省略滤镜采用双线性插值方法进行转换
    d->sws_ctx = sws_getContext(
        d->codec_ctx->width,
        d->codec_ctx->height,
        d->codec_ctx->pix_fmt,
        d->codec_ctx->width,
        d->codec_ctx->height,
        AV_PIX_FMT_RGB24,
        SWS_FAST_BILINEAR,0,0,0);

    //申请内存
    d->packet = av_packet_alloc();
    d->decode_freme = av_frame_alloc();
    d->rgb_frame = av_frame_alloc();

    int nbytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,d->codec_ctx->width,d->codec_ctx->height,1);
    d->rgb_buffer = (unsigned char*)av_mallocz(nbytes);
    av_image_fill_arrays(d->rgb_frame->data,d->rgb_frame->linesize,d->rgb_buffer,AV_PIX_FMT_RGB24,d->codec_ctx->width,d->codec_ctx->height,1);

    d->is_run = true;
    //通知线程运行
    mThread = std::thread(std::bind(&Camera::run,this));
    //m_cv.notify_one();

    return true;
}
void Camera::close() {
    if(!d->is_run)return;

    d->is_run = false;

    std::unique_lock<std::mutex> lock(mMutex);
    mCond.wait(lock,[&](){return d->is_exit == true;});

    if(mThread.joinable()){
        mThread.join();
    }

    if(d->fmt_ctx){
        avformat_close_input(&d->fmt_ctx);
        d->fmt_ctx = 0;
    }

    if(d->codec_ctx){
        avcodec_free_context(&d->codec_ctx);
        d->codec_ctx = 0;
    }

    if(d->decode_freme){
        av_frame_free(&d->decode_freme);
        d->decode_freme = 0;
    }

    if(d->rgb_buffer){
        av_free(d->rgb_buffer);
        d->rgb_buffer = nullptr;
    }

    if(d->rgb_frame){
        av_frame_free(&d->rgb_frame);
        d->rgb_frame = 0;
    }

    if(d->packet){
        av_packet_free(&d->packet);
        d->packet = 0;
    }

    if(d->sws_ctx){
        sws_freeContext(d->sws_ctx);
        d->sws_ctx = 0;
    }

    d->reset();
}
void Camera::run() {
    //QTime t;
    for(;;){
        if(!d->is_run){
            break;
        }

        //t.restart();

        //
        int ret = 0;
        ret = av_read_frame(d->fmt_ctx,d->packet);
        if(ret >= 0){
            if(d->packet->stream_index == d->video_stream_idx){

                //
                //延迟
#if 1
                AVRational *time_base = &d->fmt_ctx->streams[d->packet->stream_index]->time_base;
                double duration_time = av_q2d(*time_base) * d->packet->duration;
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(duration_time * 1000.0)));
#endif

                //if(d->packet->flags == AV_PKT_FLAG_KEY){
                if(avcodec_send_packet(d->codec_ctx,d->packet) == 0){
                    while(avcodec_receive_frame(d->codec_ctx,d->decode_freme) == 0){

                        sws_scale(d->sws_ctx,(const unsigned char * const *)d->decode_freme->data,d->decode_freme->linesize,0,d->decode_freme->height
                            ,d->rgb_frame->data,d->rgb_frame->linesize);

                        d->image = QImage((unsigned char *)d->rgb_buffer,d->decode_freme->width,d->decode_freme->height,3 * d->decode_freme->width,QImage::Format_RGB888);
                        if(!d->image.isNull()){
                            emit newData(d->image.copy());
                            //qDebug(u8"解析图片成功 %dx%d Time elapsed: %d ms",d->image.width(),d->image.height(),t.elapsed());
                        }
                    }
                }
                //}

            }
            av_packet_unref(d->packet);

        }else{
            avformat_flush(d->fmt_ctx);

            double pos = 0.0;
            long long seek_pos = d->fmt_ctx->streams[d->packet->stream_index]->duration * pos;
            av_seek_frame(d->fmt_ctx, d->packet->stream_index, seek_pos, AVSEEK_FLAG_BACKWARD);
            qDebug() << u8"未读取到任何帧 " << ret;
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(200)));
    }

    d->is_exit = true;
    mCond.notify_one();
}
