//
// Created by zemizeng on 2023/2/22.
//


#include <__locale>
#include "FFAudio.h"
#include "base.h"

void FFAudio::prepare() {

    LOGD("FFAudio prepare");
    int ret = -1;
    if (nullptr == formatContext || nullptr == codecContext) {
        LOGE("should not be like this : nullptr == formatContext || nullptr == codecContext");
        return;
    }
    ret = avcodec_open2(codecContext,
                        avcodec_find_decoder(codecParameters->codec_id), nullptr);
    LOGD("avcodec_open2 reault : %s", ret == 0 ? "success" : "fail");
    if (ret != 0) {
        return;
    }

    prepareOpenSL();
}


uint8_t *FFAudio::outAudioBuffer = nullptr;
CallJava *FFAudio::callJava = nullptr;

FFAudio::FFAudio(AVFormatContext *context,
                 AVCodecContext *context1,
                 AVCodecParameters *parameters,
                 int audioStreamIndex,
                 CallJava *cJ)
        : formatContext{context},
          codecParameters{parameters},
          codecContext{context1},
          audioQueue{new AVPacketQueue},
          audioDuration{formatContext->duration},
          audioProgress{0l},
          audioStreamIndex{audioStreamIndex} {
    callJava = cJ;
    swrContext = swr_alloc();
    AVChannelLayout outChannelLayout = AV_CHANNEL_LAYOUT_STEREO;
    AVSampleFormat outSampleFormat = AV_SAMPLE_FMT_S16;
    int outSampleRate = codecContext->sample_rate;
    swr_alloc_set_opts2(&swrContext, &outChannelLayout, outSampleFormat, outSampleRate,
                        &(codecContext->ch_layout), codecContext->sample_fmt,
                        codecContext->sample_rate,
                        0, nullptr);
    swr_init(swrContext);
    //1s的pcm数据字节数 44100*2:双通道44.1khz,16位(2字节)(想要转化的成的音频数据格式)
    outAudioBuffer = (uint8_t *) av_malloc((unsigned long) outSampleRate * 2 * 2);
}


void FFAudio::prepareOpenSL() {
    /**
     * 使用opensl es的步骤
     * 1.创建引擎(创建引擎对象，初始化，获取引擎接口)
     * 2.设置混音器
     * 3.创建播放器
     * 4.开始播放
     * 5。停止播放
     */
    SLresult result = SL_RESULT_UNKNOWN_ERROR;

    SLObjectItf engineObject = nullptr;
    result = slCreateEngine(&engineObject, 0,
                            nullptr, 0, nullptr, nullptr);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Engine Create fail");
        return;
    }
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Engine Realize fail");
        return;
    }
    SLEngineItf engineInterface = nullptr;
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineInterface);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Engine GetInterface fail");
        return;
    }
    LOGD("SL Engine initialize finish!");

    SLObjectItf mixerObject = nullptr;
    SLInterfaceID mixerInterfaceIds[1] = {SL_IID_ENVIRONMENTALREVERB};
    SLboolean mixerInterfaceRequired[]{SL_BOOLEAN_FALSE};
    result = (*engineInterface)->CreateOutputMix(engineInterface,
                                                 &mixerObject,
                                                 1, mixerInterfaceIds,
                                                 mixerInterfaceRequired);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("CreateOutputMix fail");
        return;
    }
    result = (*mixerObject)->Realize(mixerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Mixer Realize fail");
        return;
    }
    SLEnvironmentalReverbItf mixer = nullptr;
    result = (*mixerObject)->GetInterface(mixerObject, SL_IID_ENVIRONMENTALREVERB, &mixer);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Mixer GetInterface fail");
        return;
    }
    LOGD("Mixer initialize finish!");

    //player播放参数设置
    SLDataFormat_PCM formatPcm{
            .formatType=SL_DATAFORMAT_PCM,
            .numChannels=2,
            .samplesPerSec=createOpenSLSampleRate(44100),
            //采样位数16
            .bitsPerSample=SL_PCMSAMPLEFORMAT_FIXED_16,
            //和采样位数一致即可
            .containerSize=SL_PCMSAMPLEFORMAT_FIXED_16,
            .endianness=SL_BYTEORDER_LITTLEENDIAN
    };
    SLObjectItf player = nullptr;
    SLDataLocator_AndroidSimpleBufferQueue androidSimpleBufferQueue{
            .locatorType=SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            //一个采样点2字节，因为采样率为16
            .numBuffers=2
    };
    SLDataSource dataSource{
            .pLocator=&androidSimpleBufferQueue,
            .pFormat = &formatPcm
    };
    SLDataLocator_OutputMix outputMix{
            .locatorType=SL_DATALOCATOR_OUTPUTMIX,
            .outputMix=mixerObject
    };
    SLDataSink dataSink{
            .pLocator=&outputMix,
            .pFormat=nullptr
    };
    SLInterfaceID playerInterfaceIds[]{SL_IID_MUTESOLO, SL_IID_VOLUME, SL_IID_BUFFERQUEUE};
    SLboolean playerInterfaceRequired[]{SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineInterface)->CreateAudioPlayer(engineInterface, &player,
                                                   &dataSource, &dataSink,
                                                   sizeof(playerInterfaceRequired) /
                                                   sizeof(SL_BOOLEAN_TRUE),
                                                   playerInterfaceIds,
                                                   playerInterfaceRequired);
    LOGD("sizeof(playerInterfaceRequired) / sizeof(SL_BOOLEAN_TRUE) : %ld",
         sizeof(playerInterfaceRequired) / sizeof(SL_BOOLEAN_TRUE));
    if (SL_RESULT_SUCCESS != result) {
        LOGE("CreateAudioPlayer fail");
        return;
    }
    result = (*player)->Realize(player, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Player Realize fail");
        return;
    }
    SLPlayItf playerInterface = nullptr;
    result = (*player)->GetInterface(player, SL_IID_PLAY, &playerInterface);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Player GetInterface fail");
        return;
    }
    LOGD("Player initialize finish!");
    LOGD("all initialize finish!");

    SLMuteSoloItf muteInterface = nullptr;
    SLVolumeItf volumeInterface = nullptr;
    SLAndroidSimpleBufferQueueItf bufferQueueInterface = nullptr;
    result = (*player)->GetInterface(player, SL_IID_MUTESOLO, &muteInterface);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("player GetInterface SL_IID_MUTESOLO fail");
        return;
    }
    result = (*player)->GetInterface(player, SL_IID_VOLUME, &volumeInterface);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("player GetInterface SL_IID_VOLUME fail");
        return;
    }
    //callback被主动调用，用来获取需要播放的数据
    result = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &bufferQueueInterface);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("player GetInterface SL_IID_BUFFERQUEUE fail");
        return;
    }
    result = (*bufferQueueInterface)->RegisterCallback(bufferQueueInterface, slBufferQueueCallback,
                                                       this);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("bufferQueueInterface RegisterCallback fail");
        return;
    }
    //先设置为播放状态
    result = (*playerInterface)->SetPlayState(playerInterface, SL_PLAYSTATE_PLAYING);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("playerInterface SetPlayState fail");
        return;
    }
    //启动buffer queue队列回调
    result = (*bufferQueueInterface)->Enqueue(bufferQueueInterface, outAudioBuffer, 1);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("bufferQueueInterface Enqueue fail");
        return;
    }

}


void FFAudio::release() {
    isReading = false;
    isDecoding = false;
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);
    avformat_free_context(formatContext);
    avformat_close_input(&formatContext);
    av_freep(outAudioBuffer);
    swr_free(&swrContext);
    delete audioQueue;
    audioQueue = nullptr;
}

void FFAudio::start() {
    //开启读packet和decode线程
    pthread_create(&read_packet_pthread, nullptr, readingThread, this);
//    pthread_create(&decode_packet_pthread, nullptr, decodingThread, this);
    LOGD("read_packet_pthread:%ld", read_packet_pthread);
//    LOGD("decode_packet_pthread:%ld", decode_packet_pthread);
}


void *FFAudio::readingThread(void *param) {
    LOGD("readingThread start");

    auto *ffAudio = (FFAudio *) param;

    int ret = -1;
    ffAudio->isReading = true;

    while (ffAudio->isReading) {
        unsigned long packetSize = ffAudio->audioQueue->size();
        if (packetSize > 50 && ffAudio->audioQueue->isWorking()) {
            //LOGI("read audio packet size---->%ld", packetSize);
            continue;
        }
        AVPacket *packet = av_packet_alloc();
        ret = av_read_frame(ffAudio->formatContext, packet);
        if (ret < 0) {
            LOGD("maybe end of file!");
            av_packet_free(&packet);
            ffAudio->isReading = false;
            ffAudio->audioQueue->stopWorking();
            break;
        }
        //判断读出的是音频或者视频包的数据
        int index = packet->stream_index;
        AVStream *stream = ffAudio->formatContext->streams[index];
        if (stream->codecpar->codec_type
            == AVMEDIA_TYPE_AUDIO) {
            //音频包
            ffAudio->audioQueue->push(packet);
        } else if (stream->codecpar->codec_type
                   == AVMEDIA_TYPE_VIDEO) {
            //视频包
        }
    }
    ffAudio->isReading = false;
    LOGI("reading thread stop..");
    return nullptr;
}

uint32_t FFAudio::createOpenSLSampleRate(int rate) {
    uint32_t sampleRate = 0;
    switch (rate) {
        case 44100:
            sampleRate = SL_SAMPLINGRATE_44_1;
            break;
        case 12000:
            sampleRate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            sampleRate = SL_SAMPLINGRATE_16;
            break;
        case 88200:
            sampleRate = SL_SAMPLINGRATE_88_2;
            break;
        case 24000:
            sampleRate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            sampleRate = SL_SAMPLINGRATE_32;
            break;
        case 64000:
            sampleRate = SL_SAMPLINGRATE_64;
            break;
        default:
            break;
    }
    LOGD("createOpenSLSampleRate rate:%d", sampleRate);
    return sampleRate;
}

int FFAudio::decodeAudioPacket() {
    int ret = -1;
    isDecoding = true;
    while (isDecoding) {
        if (!audioQueue->isWorking() && audioQueue->size() == 0) {
            break;
        }
        if (audioQueue->size() == 0) {
            continue;
        }
        AVPacket *packet = av_packet_alloc();
        audioQueue->pull(packet);
        LOGI("1.pkt pts:%ld,pkt time_base:%f,pkt duration:%ld,stream time_base:%f,context time_base:%f",
             packet->pts,
             av_q2d(packet->time_base),
             packet->duration,
             av_q2d(formatContext->streams[audioStreamIndex]->time_base),
             av_q2d(codecContext->time_base));

        ret = avcodec_send_packet(codecContext, packet);
        if (0 != ret) {
            LOGE("avcodec_send_packet fail : %d", ret);
            av_packet_free(&packet);
            continue;
        }
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);
        LOGI("2.frame pts:%ld,frame time_base:%f,frame duration:%ld,progress:%f,best progress:%f",
             frame->pts,
             av_q2d(frame->time_base),
             frame->duration,
             av_q2d(formatContext->streams[audioStreamIndex]->time_base) * frame->pts,
             av_q2d(formatContext->streams[audioStreamIndex]->time_base) * frame->best_effort_timestamp);
        if (0 != ret) {
            LOGE("avcodec_receive_frame fail : %d", ret);
            av_packet_free(&packet);
            av_frame_free(&frame);
            continue;
        }
        //do something with decode data
        int oneChannelNbSamples = swr_convert(swrContext, &outAudioBuffer,
                                              frame->nb_samples,
                                              (const uint8_t **) frame->data,
                                              frame->nb_samples);
        if (0 > oneChannelNbSamples) {
            av_packet_free(&packet);
            av_frame_free(&frame);
            continue;
        }
        //LOGI("frame->nb_samples----->%d , oneChannelNbSamples---->%d",
        //frame->nb_samples,
        //oneChannelNbSamples);
        int actualDataSize = oneChannelNbSamples * 2 * 2;
        //free
        isDecoding = false;
        av_packet_free(&packet);
        av_frame_free(&frame);
        return actualDataSize;
    }
    return 0;
}


//底层调用就证明需要数据，然后我们就读packet然后解码packet然后再把解码好的pcm数据传给底层
void FFAudio::slBufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *pContext) {
    //LOGI("slBufferQueueCallback..............................................");
    auto ffAudio = (FFAudio *) pContext;
    int bufferSize = ffAudio->decodeAudioPacket();
    SLuint32 result = SL_RESULT_UNKNOWN_ERROR;
    result = (*bufferQueue)->Enqueue(bufferQueue, outAudioBuffer, (SLuint32) bufferSize);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("slBufferQueueCallback bufferQueueInterface Enqueue fail");
        return;
    }
    //callJava->CallVoidMethodV(ThreadWhere::CHILD_THREAD,"progress","()V");
}







