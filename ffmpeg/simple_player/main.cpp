//
// Created by fifthdimension on 2020/3/2.
//

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
// #include "libswscale/swscale_internal.h"
}

#include <SDL2/SDL.h>

#include <queue>

#define SDL_AUDIO_BUFFER_SIZE (1152)           // ??
#define AVCODEC_MAX_AUDIO_FRAME_SIZE (192000)  // ??

using namespace std;

//创建一个全局的结构体变量以便于我们从文件中得到的声音包有地方存
//放同时也保证SDL中的声音回调函数audio_callback 能从这个地方得到声音数据
typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    // 如果我们没有正确的锁定这个队列，我们有可能把数据搞乱。
    //是在一个独立的线程中来进行音频处理的。
    //互斥锁; 因为SDL
    SDL_mutex *mutex;
    SDL_cond *cond;  //条件变量
} PacketQueue;

PacketQueue audioQ;
PacketQueue videoQ;
queue<AVFrame *> frameQueue;  // C++的std queue 渲染视频使用队列

static int quit = 0;        // app 退出

void audio_callback(void *userdata, Uint8 *stream, int len);

int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audioBuffer, int bufSize);

/**
 *
 *
 */
int audioResampling(AVCodecContext *codecCtx, AVFrame *pAudioDecodeFrame,
                    int out_sample_fmt, int out_channels, int out_sample_rate,
                    uint8_t *out_buf) {
    SwrContext *swr_ctx = nullptr;  // SwrContext重采样结构体
    int data_size = 0;
    int ret = 0;
    int64_t src_ch_layout = codecCtx->channel_layout;
    int64_t dst_ch_layout = AV_CH_LAYOUT_STEREO;
    int dst_nb_channels = 0;
    int dst_linesize = 0;
    int src_nb_samples = 0;
    int dst_nb_samples = 0;
    int max_dst_nb_samples = 0;
    uint8_t **dst_data = nullptr;
    int resampled_data_size = 0;

    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        printf("swr_alloc error \n");
        return -1;
    }

    src_ch_layout = (codecCtx->channels == av_get_channel_layout_nb_channels(codecCtx->channel_layout))
                    ? codecCtx->channel_layout
                    : av_get_default_channel_layout(codecCtx->channels);

    if (out_channels == 1) {
        dst_ch_layout = AV_CH_LAYOUT_MONO;
        // printf("dst_ch_layout: AV_CH_LAYOUT_MONO\n");
    } else if (out_channels == 2) {
        dst_ch_layout = AV_CH_LAYOUT_STEREO;
        // printf("dst_ch_layout: AV_CH_LAYOUT_STEREO\n");
    } else {
        dst_ch_layout = AV_CH_LAYOUT_SURROUND;
        // printf("dst_ch_layout: AV_CH_LAYOUT_SURROUND\n");
    }

    if (src_ch_layout <= 0) {
        printf("src_ch_layout error \n");
        return -1;
    }

    src_nb_samples = pAudioDecodeFrame->nb_samples;
    if (src_nb_samples <= 0) {
        printf("src_nb_samples error \n");
        return -1;
    }
    return 0;
}

static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block) {
    AVPacketList *pkt1;
    int ret;
    SDL_LockMutex(q->mutex);

    for (;;) {
        if (quit) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt) q->last_pkt = nullptr;
            q->nb_packets--;
            q->size -= pkt1->pkt.size;
            //*pkt = pkt1->pkt;
            av_packet_ref(pkt, &pkt1->pkt);
            av_packet_unref(&pkt1->pkt);
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            SDL_CondWait(q->cond, q->mutex);
        }
    }
    SDL_UnlockMutex(q->mutex);
    return ret;
}

int main() {
    const char *filePath = "../../media/test.mp4";
    unique_ptr<AVFormatContext, void (*)(AVFormatContext *)> avFormatCtx{
            nullptr,
            [](AVFormatContext *ctx) {
                printf("%s %s\n", "释放", "avFormatCtx");
                avformat_close_input(&ctx);
            }
    };
    {
        AVFormatContext *tmp = nullptr;
        printf("%s %s\n", "播放文件", filePath);
        // Open an input stream and read the header
        if (avformat_open_input(&tmp, filePath, nullptr, nullptr) != 0) {
            printf("Can't open the file\n");
            return -1;
        }
        avFormatCtx.reset(tmp);
    }
    // region 检测媒体格式，查找音视频文件索引
    // 提取流信息
    if (avformat_find_stream_info(avFormatCtx.get(), nullptr) < 0) {
        printf("Couldn't find stream information.\n");
        return -1;
    }
    // 打印流信息
    av_dump_format(avFormatCtx.get(), 0, filePath, 0);

    // 统计音视频所在索引
    int i, videoIndex, audioIndex;
    videoIndex = -1;
    audioIndex = -1;
    // nb_streams 是流个数, 分析流是视频还是音频 保留索引
    // 4.0 codec过时 使用codecpar
    for (i = 0; i < avFormatCtx->nb_streams; i++) {  //视音频流的个数
        if (avFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
            videoIndex < 0) {
            videoIndex = i;
        }
        if (avFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
            audioIndex < 0)
            audioIndex = i;
    }
    printf("%s \n", "索引");
    printf("%s %d\n", "videoIndex =", videoIndex);
    printf("%s %d\n", "audioIndex =", audioIndex);

    // 没有音视频就退出
    if (videoIndex == -1) return -1;
    if (audioIndex == -1) return -1;
    // endregion

    auto vCodecParam = avFormatCtx->streams[videoIndex]->codecpar;
    auto aCodecParam = avFormatCtx->streams[audioIndex]->codecpar;
    auto vCodec = avcodec_find_decoder(vCodecParam->codec_id);
    auto aCodec = avcodec_find_decoder(aCodecParam->codec_id);

    unique_ptr<AVCodecContext, void (*)(AVCodecContext *)> vCodecCtx{avcodec_alloc_context3(vCodec),
                                                                     [](AVCodecContext *ctx) { avcodec_close(ctx); }};
    avcodec_parameters_to_context(vCodecCtx.get(), vCodecParam);

    unique_ptr<AVCodecContext, void (*)(AVCodecContext *)> aCodecCtx{avcodec_alloc_context3(aCodec),
                                                                     [](AVCodecContext *ctx) { avcodec_close(ctx); }};
    avcodec_parameters_to_context(aCodecCtx.get(), aCodecParam);

    if (vCodec == nullptr || aCodec == nullptr) {
        printf("Unsupported codec!\n");
        return -1;
    }
    // 打开视频解码器
    if (avcodec_open2(vCodecCtx.get(), vCodec, nullptr) < 0) {
        printf("Could not open video codec.\n");
        return -1;
    }
    // 打开音频解码器
    if (avcodec_open2(aCodecCtx.get(), aCodec, nullptr) < 0) {
        printf("Could not open audio codec.\n");
        return -1;
    }

    auto pFrame = av_frame_alloc();
    auto pFrameYUV = av_frame_alloc();

    //即使我们申请了一帧的内存，当转换的时候，我们仍然需要一个地方来放置原始
    //的数据。我们使用 av_image_get_buffer_size 来获得我们需要的大小，
    // 然后手工申请内存空间：
    // 格式*宽*高 = 内存大小
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, vCodecParam->width,
                                            vCodecParam->height, 1 /*(按照多少字节对齐)*/);
    auto outBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    // 关联 frame 和我们刚才分配的内存
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, outBuffer,
                         AV_PIX_FMT_YUV420P, vCodecCtx->width, vCodecCtx->height, 1);

    //----------------SDL--------------------------------------//
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf("Could not initialize SDL -%s\n", SDL_GetError());
        exit(1);
    }

    //先设置声音的选项：采样率，声音通道数和其它的参数，
    //然后设置一个回调函数和一些用户数据userdata。当开始播放音频的时候，
    // SDL
    // 将不断地调用这个回调函数并且要求它来向声音缓冲填入一个特定的数量的字节。

    //把这些信息放到SDL_AudioSpec结构体中后，
    //调用函数SDL_OpenAudio()就会打开声音设备并且给我们送回另外一个AudioSpec
    //结构体。 这个结构体是我们实际上用到的－－因为我们不能保证得到我们所要求的。
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = aCodecParam->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = aCodecParam->channels;  //声音的通道数
    wanted_spec.silence = 0;                        //用来表示静音的值
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;    //声音缓冲区的大小
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = aCodecCtx.get();
}


/**
 * 声音回调函数
 * userdata是输入，stream是输出，len是输入，len的值一般为4096（调试中发现的）
 * audio_callback函数的功能是调用audio_decode_frame函数，把解码后数据块audio_buf追加在stream的后面，
 * 通过SDL库对audio_callback的不断调用，不断解码数据，然后放到stream的末尾，
 * SDL库认为stream中数据够播放一帧音频了，就播放它,
 * 第三个参数len是向stream中写数据的内存分配尺度，是分配给audio_callback函数写入缓存大小。
 */
void audio_callback(void *userdata, Uint8 *stream, int len) {
    // //SDL_memset(stream, 0, len);
    auto aCodecCtx = (AVCodecContext *) userdata;  //??
    int offset, audio_size;

    // //audio_buf 的大小为 1.5 倍的声音帧的大    小以便于有一个比较好的缓冲
    static uint8_t audioBuffer[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    static unsigned int audio_buf_size = 0;
    static unsigned int audio_buf_index = 0;

    while (len > 0) { // 输出流长度大于0，为输出流赋值
        if (audio_buf_index >= audio_buf_size) {  // 索引大于数据，说明,是第一次循环没有buffer
            // already send all our data, get more
            audio_size = audio_decode_frame(aCodecCtx, audioBuffer, sizeof(audioBuffer));
            if (audio_size < 0) {  // error, output silence
                printf("error, output silence\n");
                audio_buf_size = SDL_AUDIO_BUFFER_SIZE;
                memset(audioBuffer, 0, audio_buf_size);
            } else {
                audio_buf_size = audio_size;
            }
            audio_buf_index = 0;
        }

        // 每次循环，计算偏移。
        offset = audio_buf_size - audio_buf_index;
        if (offset > len) {
            offset = len;
        }

        memcpy(stream, (uint8_t *) audioBuffer + audio_buf_index, offset);
        len -= offset;
        stream += offset;
        audio_buf_index += offset;
    }
}

/**
 * 把数据存储在一个中间缓冲中，企图将字节转变为流，当我们数据不够的时候提供给我们，
 * 当数据塞满时帮我们保存数据以使我们以后再用。
 */
int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audioBuffer, int bufSize) {
    static AVPacket pkt;
    static uint8_t *audio_pkt_data = nullptr;
    static int audio_pkt_size = 0;

    int len1, data_size = 0;
    //   int ret = 0;

    static AVFrame *pFrame;
    pFrame = av_frame_alloc();

    for (;;) {
        while (audio_pkt_size > 0) {
            data_size = bufSize;

            int res = avcodec_send_packet(aCodecCtx, &pkt);
            if (res != 0) {
                printf("error\n");
                audio_pkt_size = 0;
                break;
            }
            len1 = avcodec_receive_frame(aCodecCtx, pFrame);

            //   len1 = avcodec_decode_audio4(aCodecCtx, pFrame, &ret, &pkt);

            // len1 = avcodec_decode_audio3(aCodecCtx, (int16_t *)audio_buf,
            //    &data_size, &pkt);
            if (len1 < 0) {  // if error, skip frame
                printf("error\n");
                audio_pkt_size = 0;
                break;
            }
            data_size = audioResampling(aCodecCtx, pFrame, AV_SAMPLE_FMT_S16, 2,
                                        44100, audioBuffer);
            audio_pkt_data += len1;
            audio_pkt_size -= len1;
            if (data_size <= 0)  // No data yet, get more frames
                continue;
            return data_size;
        }
        // if (pkt.data)
        av_packet_unref(&pkt);
        if (quit) return -1;
        if (packet_queue_get(&audioQ, &pkt, 1) < 0) {  //从这里开始，取得main线程放入队列的包
            printf("error, can't get packet from the queue");
            return -1;
        }

        // SDL_LockMutex(g_pVideoMutex);
        audio_pts = pkt.pts;
        audio_dts = pkt.dts;
        // SDL_UnlockMutex(g_pVideoMutex);

        audio_pkt_data = pkt.data;
        audio_pkt_size = pkt.size;
    }
}
