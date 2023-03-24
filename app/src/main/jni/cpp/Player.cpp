//
// Created by mingzz on 2023/2/8.
//
#include "player.h"
#include "base.h"
//ffmpeg use C
extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}

#include "av_packet_queue.h"

void *reading_packet(void *);

void *videoDecoding_packet(void *);

void *audioDecoding_packet(void *);

AVFormatContext *avFormatContext;
AVCodecContext *videoCodecContext;
AVCodecContext *audioCodecContext;
ANativeWindow *aNativeWindow;
SwsContext *swsContext; //视频格式转换上下文
SwrContext *swrContext; //音频格式转换上下文
AVPacketQueue *videoPacketQueue;
AVPacketQueue *audioPacketQueue;
uint8_t *outVideoBuffer;
uint8_t *outAudioBuffer;
AVFrame *tempVideoFrame;

int videoStreamIndex = -1;
int audioStreamIndex = -1;
bool readingStart = false;
bool audioStart = false;
bool videoStart = false;

//回调java方法需要的全局变量
JNIEnv *jniEnv; //主线程
JNIEnv *jniEnv1; //解码音频的子线程
JavaVM *vm; //java vm 环境
jmethodID jmethodId1; //java method id(FFmpegActivity:prepareAudioTrack)
jmethodID jmethodId2; //java method id(FFmpegActivity:audioTrackWrite)
jobject javaObject; //全局java对象

void prepareMainThreadJavaMethod(JavaVM *javaVm, JNIEnv *env, jobject thiz) {
    vm = javaVm;
    jniEnv = env;
    javaObject = env->NewGlobalRef(thiz);
    jclass objectClass = env->GetObjectClass(thiz);
    jmethodId1 = env->GetMethodID(objectClass, "prepareAudioTrack", "(III)V");
}

void prepareChildThreadJavaMethod() {
    vm->GetEnv((void **) jniEnv1, JNI_VERSION_1_6);
    vm->AttachCurrentThread(&jniEnv1, nullptr);
    jclass jclazz = jniEnv1->GetObjectClass(javaObject);
    jmethodId2 = jniEnv1->GetMethodID(jclazz, "audioTrackWrite", "([BI)V");
}

void releaseRef() {
    LOGI("release!");
    jniEnv->DeleteGlobalRef(javaObject);
}


void Player::getStreamInfo(JNIEnv *env, const char *filePath, jobject thiz, jobject surface) {

    LOGI("视频文件路径：%s", filePath);

    prepareMainThreadJavaMethod(javaVm, env, thiz);

    int ret = -1;

    //初始化ffmpeg网络模块，以便于播放网络数据流
    avformat_network_init();

    avFormatContext = avformat_alloc_context();

    avformat_open_input(&avFormatContext, filePath, nullptr, nullptr);

    ret = avformat_find_stream_info(avFormatContext, nullptr);

    if (ret < 0) {
        return;
    }

    long duration = avFormatContext->duration;
    LOGI("视频文件总时长：%ld ns", duration);

    unsigned int nb_streams = avFormatContext->nb_streams;
    LOGI("一共有%d个stream数据", nb_streams);

    //查找音频流和视频流
    for (int i = 0; i < nb_streams; ++i) {
        AVStream *avStream = avFormatContext->streams[i];
        AVMediaType mediaType = avStream->codecpar->codec_type;
        switch (mediaType) {
            //音频流
            case AVMEDIA_TYPE_AUDIO:
                LOGI("音频流 id ：%d index : %d", avStream->id, i);
                audioStreamIndex = i;
                break;
                //视频流
            case AVMEDIA_TYPE_VIDEO:
                videoStreamIndex = i;
                LOGI("视频流 id ：%d index : %d", avStream->id, i);
                break;
            case AVMEDIA_TYPE_UNKNOWN:
                break;
            case AVMEDIA_TYPE_DATA:
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                break;
            case AVMEDIA_TYPE_ATTACHMENT:
                break;
            case AVMEDIA_TYPE_NB:
                break;
        }
    }

    //视频相关信息
    AVStream *videoStream = avFormatContext->streams[videoStreamIndex];

    int width = videoStream->codecpar->width;
    int height = videoStream->codecpar->height;
    LOGI("视频流宽高：%d X %d", width, height);

    //视频流数据的参数信息
    AVCodecParameters *videoCodecParameters = videoStream->codecpar;

    AVCodecID videoCodecId = videoCodecParameters->codec_id;
    const char *videoCodecName = avcodec_get_name(videoCodecId);
    LOGI("视频流编码器：%s", videoCodecName);

    int fps = videoStream->r_frame_rate.num;
    LOGI("视频流帧率：%d fps", fps);

    long nb_frames = videoStream->nb_frames;
    LOGI("视频流总共帧数：%ld", nb_frames);

    //先初始化并配置视频解码器
    const AVCodec *avCodec = avcodec_find_decoder(videoCodecId);
    videoCodecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(videoCodecContext, videoCodecParameters);
    //打开解码器
    ret = avcodec_open2(videoCodecContext, avCodec, nullptr);
    LOGI("视频解码器打开:%s!", ret == 0 ? "成功" : "失败");

    //音频相关信息
    AVStream *audioStream = avFormatContext->streams[audioStreamIndex];
    AVCodecParameters *audioCodecParameters = audioStream->codecpar;
    AVCodecID audioCodecID = audioCodecParameters->codec_id;
    const char *audioCodecIDC = avcodec_get_name(audioCodecID);
    LOGI("audio codec ID-->%s", audioCodecIDC);

    //先初始化并配置音频解码器
    const AVCodec *audioCodec = avcodec_find_decoder(audioCodecID);
    audioCodecContext = avcodec_alloc_context3(audioCodec);
    avcodec_parameters_to_context(audioCodecContext, audioCodecParameters);

    //采样位数和采样率
    int sampleBits = audioCodecContext->bits_per_coded_sample;
    int sampleRate = audioCodecContext->sample_rate;
    int channels = audioCodecParameters->ch_layout.nb_channels;
    LOGI("通道数,音频采样率,采样位数：%d,%d,%d", channels, sampleRate, sampleBits);

    //打开解码器
    ret = avcodec_open2(audioCodecContext, audioCodec, nullptr);
    LOGI("音频解码器打开:%s!", ret == 0 ? "成功" : "失败");

    prepareANativeWindow(surface);

    //最终要转化成44.1khz 2channels 16bits 格式送给AudioTrack播放
    prepareAudioTrack(sampleRate, 2, 16);

    //开启读流数据包和解码的线程
    readingStart = true;
    audioStart = true;
    videoStart = true;
    pthread_t readPacket_t;
    pthread_t videoDecodePacket_t;
    pthread_t audioDecodePacket_t;
    pthread_create(&readPacket_t, nullptr, reading_packet, nullptr);
    pthread_create(&videoDecodePacket_t, nullptr, videoDecoding_packet, nullptr);
    pthread_create(&audioDecodePacket_t, nullptr, audioDecoding_packet, nullptr);

}

void Player::prepareANativeWindow(jobject surface) {
    //配置好Surface对应好的NativeWindow(android库里面的)
    aNativeWindow = ANativeWindow_fromSurface(jniEnv, surface);
    //Change the format and size of the window buffers
    int ret = ANativeWindow_setBuffersGeometry(aNativeWindow, videoCodecContext->width,
                                               videoCodecContext->height,
                                               AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM);
    LOGI("Native Window config %s:%d", ret == 0 ? "success" : "fail", ret);

    //转换 videoCodecContext->pix_fmt ---> AV_PIX_FMT_RGBA(R8G8B8A)
    //surface目前只支持rgb的数据格式所以需要转换
    tempVideoFrame = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(
            AV_PIX_FMT_RGBA, videoCodecContext->width,
            videoCodecContext->height, 1);

    outVideoBuffer = (uint8_t *) av_malloc(numBytes);
    //相当于把outVideoBuffer用AVFrame来表示，tempFrame->data 其实就是指向outVideoBuffer的
    //tempFrame->data指针数组指向多个颜色通道，并且tempFrame->linesize分别表示各个通道一行width的像素的字节数（也就是data一行width数据的大小）
    //也有可能出现位对齐的情况，这时比如对于yuv420p来说tempFrame->linesize可能是大于width
    av_image_fill_arrays(tempVideoFrame->data,
                         tempVideoFrame->linesize,
                         outVideoBuffer,
                         AV_PIX_FMT_RGBA,
                         videoCodecContext->width,
                         videoCodecContext->height,
                         1);
    //创建转换context
    swsContext = sws_getContext(videoCodecContext->width,
                                videoCodecContext->height,
                                videoCodecContext->pix_fmt,
                                videoCodecContext->width,
                                videoCodecContext->height,
                                AV_PIX_FMT_RGBA,
                                SWS_BICUBIC, NULL, NULL, NULL);

}

void displayOnSurface(AVFrame *avFrame) {

    ANativeWindow_Buffer windowBuffer;
    ANativeWindow_lock(aNativeWindow, &windowBuffer, NULL);
    sws_scale(swsContext,
              avFrame->data,
              avFrame->linesize, 0,
              videoCodecContext->height,
              tempVideoFrame->data,
              tempVideoFrame->linesize);

    uint8_t *windowBuffer_ = static_cast<uint8_t *>(windowBuffer.bits);

    //把解码出来的AVFrame数据copy到NativeWindow的buffer中，然后让SurfaceView去display
    for (int i = 0; i < videoCodecContext->height; ++i) {
        memcpy(windowBuffer_ + i * windowBuffer.stride * 4,
               outVideoBuffer + i * tempVideoFrame->linesize[0],
               tempVideoFrame->linesize[0]);
    }

    ANativeWindow_unlockAndPost(aNativeWindow);

}

void Player::prepareAudioTrack(int32_t sampleRate, int32_t channels,
                               int32_t encodingFormat) {

    swrContext = swr_alloc();
    AVChannelLayout outChannelLayout = AV_CHANNEL_LAYOUT_STEREO;
    AVSampleFormat outSampleFormat = AV_SAMPLE_FMT_S16;
    int outSampleRate = sampleRate;
    swr_alloc_set_opts2(&swrContext, &outChannelLayout, outSampleFormat, outSampleRate,
                        &(audioCodecContext->ch_layout), audioCodecContext->sample_fmt, sampleRate,
                        0, nullptr);
    swr_init(swrContext);

    //1s的pcm数据字节数 44100*2:双通道44.1khz,16位(想要转化的成的音频数据格式)
    outAudioBuffer = (uint8_t *) av_malloc(sampleRate * 2 * 2);

    jniEnv->CallVoidMethod(javaObject, jmethodId1, sampleRate, channels, encodingFormat);
}

void Player::destroy() {
    releaseRef();
}

void audioTrackPlay(AVFrame *avFrame) {

    int oneChannelFrameSamplesSize = swr_convert(swrContext, &outAudioBuffer,
            //avFrame->linesize[0]不代表音频frame samples所占用的有效字节大小
            //比如 ：avFrame->linesize[0]:8192 nb_samples:960 format:float(32bits) channels:1
                                                 avFrame->linesize[0] + avFrame->linesize[1],
                                                 (const uint8_t **) avFrame->data,
                                                 avFrame->nb_samples);
    int oneFrameSamplesBufferSize = av_samples_get_buffer_size(nullptr, 2,
                                                               avFrame->nb_samples,
                                                               AV_SAMPLE_FMT_S16, 1);
    //或者这样算也是一样的，因为我们转换的音频格式有效数据(44.1khz,2ch,16bits:2bytes)
    //nb_samples是解码这帧数据的样本数量，不是一秒的样本数量！！
    //int samplesBufferSize = avFrame->nb_samples * 2 * 2;

    jbyteArray jbytes = jniEnv1->NewByteArray(oneFrameSamplesBufferSize);
    jniEnv1->SetByteArrayRegion(jbytes, 0, oneFrameSamplesBufferSize, (jbyte *) outAudioBuffer);
    //把buffer传给java层的AudioTrack播放
    LOGI("oneFrameSamplesBufferSize------>%d", oneFrameSamplesBufferSize);
    jniEnv1->CallVoidMethod(javaObject, jmethodId2, jbytes, oneFrameSamplesBufferSize);
    jniEnv1->DeleteLocalRef(jbytes);
}

void *reading_packet(void *params) {
    LOGI("reading packet线程已开启！");

    int ret = -1;

    if (nullptr == avFormatContext) {
        LOGI("avFormatContext未初始化 : is null !");
        return nullptr;
    }

    videoPacketQueue = new AVPacketQueue();

    audioPacketQueue = new AVPacketQueue();

    while (readingStart) {
        if (videoPacketQueue->size() > 100 || audioPacketQueue->size() > 100) {
            LOGI("来不及消费了，video queue,audio queue size:%ld,%ld",
                 videoPacketQueue->size(),
                 audioPacketQueue->size());
            //消费者来不及消费，需要暂停下生产
            usleep(100 * 1000);
        }
        //alloc出来的packet在queue那边被pull出来的时候才被free
        AVPacket *avPacket = av_packet_alloc();
        //return 0 if OK, < 0 on error or end of file
        ret = av_read_frame(avFormatContext, avPacket);
        if (ret < 0) {
            LOGI("av_read_frame is error or end of file");
            videoPacketQueue->stopWorking();
            audioPacketQueue->stopWorking();
            break;
        }
        int stream_index = avPacket->stream_index;
        if (videoStreamIndex == stream_index) {
            LOGI("the packet is video packet,pts:%ld", avPacket->pts);
            //放入安全队列中
            videoPacketQueue->push(avPacket);
        } else if (audioStreamIndex == stream_index) {
            LOGI("the packet is audio packet,pts:%ld", avPacket->pts);
            audioPacketQueue->push(avPacket);
        }
    }

    return nullptr;
}

void *videoDecoding_packet(void *) {
    LOGI("video decoding packet线程已开启！");

    int ret = -1;

    while (videoStart) {
        //从queue里面拿packet
        AVPacket *avPacket = av_packet_alloc();
        videoPacketQueue->pull(avPacket);
        if (!videoPacketQueue->isWorking() && videoPacketQueue->size() == 0) {
            videoStart = false;
            LOGI("可以停止解码工作了！");
        }
        //把packet送入编码器
        ret = avcodec_send_packet(videoCodecContext, avPacket);
        if (ret != 0) {
            LOGI("avcodec_send_packet fail ret:%d!", ret);
            av_packet_free(&avPacket);
            continue;
        }
        //拿编码好的frame
        AVFrame *avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(videoCodecContext, avFrame);
        if (ret != 0) {
            LOGI("avcodec_send_packet fail ret:%d!", ret);
            av_packet_free(&avPacket);
            av_frame_free(&avFrame);
            continue;
        }
        LOGI("avFrame format:%d pts:%ld", avFrame->format, avFrame->pts);

        displayOnSurface(avFrame);

        av_packet_free(&avPacket);
        av_frame_free(&avFrame);
    }

    av_frame_free(&tempVideoFrame);

    free(outVideoBuffer);

    avcodec_close(videoCodecContext);

    sws_freeContext(swsContext);

    delete videoPacketQueue;

    return nullptr;
}

void *audioDecoding_packet(void *params) {
    LOGI("audio decoding packet线程已开启！");

    prepareChildThreadJavaMethod();

    int ret = -1;

    while (audioStart) {
        //从queue里面拿packet
        AVPacket *avPacket = av_packet_alloc();
        audioPacketQueue->pull(avPacket);
        if (!audioPacketQueue->isWorking() && audioPacketQueue->size() == 0) {
            audioStart = false;
            LOGI("可以停止解码工作了！");
        }
        //把packet送入编码器
        ret = avcodec_send_packet(audioCodecContext, avPacket);
        if (ret != 0) {
            LOGI("avcodec_send_packet fail ret:%d!", ret);
            av_packet_free(&avPacket);
            continue;
        }
        //拿编码好的frame
        AVFrame *avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(audioCodecContext, avFrame);
        if (ret != 0) {
            LOGI("avcodec_send_packet fail ret:%d!", ret);
            av_packet_free(&avPacket);
            av_frame_free(&avFrame);
            continue;
        }
        LOGI("avFrame format:%d pts:%ld", avFrame->format, avFrame->pts);

        //do something
        audioTrackPlay(avFrame);

        av_packet_free(&avPacket);
        av_frame_free(&avFrame);
    }

    free(outAudioBuffer);

    delete audioPacketQueue;

    swr_free(&swrContext);

    vm->DetachCurrentThread();

    return nullptr;
}




