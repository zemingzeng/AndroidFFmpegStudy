//
// Created by zemizeng on 2023/2/22.
//

#ifndef FFMPEG_FFAUDIO_H
#define FFMPEG_FFAUDIO_H

//ffmpeg use C
extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
//opensl es
#include "SLES/OpenSLES_Android.h"
#include <SLES/OpenSLES.h>
}


#include "av_packet_queue.h"
#include "CallJava.h"

class FFAudio {

public:
    void prepare();

    void start();

    void prepareOpenSL();

    u_int32_t createOpenSLSampleRate(int rate);

    FFAudio(AVFormatContext *context, AVCodecContext *context1,
            AVCodecParameters *parameters,int audioStreamIndex,CallJava* callJava);

    int decodeAudioPacket();

    void release();

private:

    static void slBufferQueueCallback(
            SLAndroidSimpleBufferQueueItf caller,
            void *pContext
    );

    static void *readingThread(void *param);

    //static void *decodingThread(void *param);

private:
    static uint8_t *outAudioBuffer;
    static CallJava *callJava;

private:
    AVCodecParameters *codecParameters;
    //pthread_t decode_packet_pthread;
    pthread_t read_packet_pthread;
    AVCodecContext *codecContext;
    AVFormatContext *formatContext;
    SwrContext *swrContext;
    AVPacketQueue *audioQueue;
    int audioStreamIndex;
    long audioDuration;
    long audioProgress;
    bool isReading;
    bool isDecoding;
};

#endif //FFMPEG_FFAUDIO_H


