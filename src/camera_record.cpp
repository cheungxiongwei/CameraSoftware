//
// Created by cheungxiongwei on 2022/3/29.
//

#include "camera_record.h"
//ffmpeg
#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"         //编码/解码库
#include "libavdevice/avdevice.h"       //特殊设备复合/复制库
#include "libavfilter/avfilter.h"       //基于图形的框架编辑库
#include "libavformat/avformat.h"       // I/O 格式库
#include "libavutil/avutil.h"           //所有FFmpeg库共享的通用代码
#include <libavutil/imgutils.h>
#include "libpostproc/postprocess.h"    //视频后处理库
#include "libswresample/swresample.h"   //音频处理重采样，样本格式转换和混合的库
#include "libswscale/swscale.h"         //颜色转换和缩放库
#ifdef __cplusplus
}
#endif

static double timestamp(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational time_base = fmt_ctx->streams[pkt->stream_index]->time_base;
    return av_q2d(time_base) * pkt->dts;
}

class CameraRecord_
{
public:
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;

    int stream_index = 0;
    int *stream_mapping = NULL;
    int stream_mapping_size = 0;
};


CameraRecord::CameraRecord(std::function<bool(qreal)> func, QObject* parent): QObject(parent) {
    mCallbackFunc = func;
}

CameraRecord::~CameraRecord() {
    stop();
}

int CameraRecord::open(const std::string& in_filename, const std::string& out_filename) {
    using namespace std::placeholders;
    if (!mThread.joinable()) {
        mThread = std::thread(std::bind(&CameraRecord::run, this, _1, _2), in_filename, out_filename);
    }
    return 0;
}

void CameraRecord::stop() {
    if (mThread.joinable()) {
        mThread.join();
    }
}

int CameraRecord::run(const std::string& in_filename, const std::string& out_filename) {
    const char *str_in_filename = in_filename.data();
    const char *str_out_filename = out_filename.data();

    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;

    int stream_index = 0;
    int *stream_mapping = NULL;
    int stream_mapping_size = 0;
    int ret = -1;

    int64_t temp_pts = 0;
    int64_t temp_dts = 0;

    AVDictionary *dict = NULL;
    //设置rtsp流已tcp协议打开
    av_dict_set(&dict, "rtsp_transport", "tcp", 0);

    //网络延时时间
    av_dict_set(&dict, "max_delay", "200", 0);
    av_dict_set(&dict, "stimeout", "3000000", 0);

    if ((ret = avformat_open_input(&ifmt_ctx, str_in_filename, 0, &dict)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", str_in_filename);
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        goto end;
    }

    av_dump_format(ifmt_ctx, 0, str_in_filename, 0);

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, str_out_filename);
    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = static_cast<int*>(av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping)));
    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    ofmt = ofmt_ctx->oformat;

    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *out_stream;
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }

        stream_mapping[i] = stream_index++;

        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            fprintf(stderr, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0) {
            fprintf(stderr, "Failed to copy codec parameters\n");
            goto end;
        }
        out_stream->codecpar->codec_tag = 0;
    }
    av_dump_format(ofmt_ctx, 0, str_out_filename, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, str_out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", str_out_filename);
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        goto end;
    }

    while (1) {
        AVStream *in_stream, *out_stream;

        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;

        in_stream = ifmt_ctx->streams[pkt.stream_index];
        if (pkt.stream_index >= stream_mapping_size ||
            stream_mapping[pkt.stream_index] < 0) {
            av_packet_unref(&pkt);
            continue;
        }

        pkt.stream_index = stream_mapping[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        //log_packet(ifmt_ctx, &pkt, "in");

        /* copy packet */
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //log_packet(ofmt_ctx, &pkt, "out");

        if (pkt.pts < temp_pts || pkt.dts < temp_dts) {
            av_packet_unref(&pkt);
            continue;
        }
        temp_pts = pkt.pts;
        temp_dts = pkt.dts;

        //获取当前时间戳
        double temp_timestamp = timestamp(ofmt_ctx, &pkt);
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error muxing packet\n");
            break;
        }
        av_packet_unref(&pkt);

        emit timestampChanged(temp_timestamp);
        if (!mCallbackFunc(temp_timestamp)) {
            break;
        }
    }

    av_write_trailer(ofmt_ctx);
    end:

    avformat_close_input(&ifmt_ctx);

    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);

    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF) {
        //fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return 0;
}
