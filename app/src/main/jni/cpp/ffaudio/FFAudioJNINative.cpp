//
// Created by zemizeng on 2023/2/22.
//

#include "base.h"
#include "FFAudio.h"
#include "CallJava.h"
#include "test.h"

bool IS_TEST = false;

void test(JNIEnv *env, jobject thiz) {
    if (IS_TEST) {
        test_();
        LOGI("Welcome to ffAudio jni native test using c++ version : %ld!!", __cplusplus);
        try {
            if (1)
                throw invalid_argument{"i am throwing a exception!!!!!!!!!!!!!!!!!!!!!!"};
        } catch (const invalid_argument &exception) {
            LOGI("exception:%s", exception.what());
        }
    }
}


FFAudio *ffAudio = nullptr;
CallJava *callJava = nullptr;
//需要在结束时delete掉这个global reference
jobject globalObject;


void initCallJava(JNIEnv *env, jobject thiz) {
    if (IS_TEST)
        return;
    LOGI("initCallJava");
    //初始化callJava变量
    callJava->mainThreadEnv = env;
    callJava->globalJavaObject = globalObject = env->NewGlobalRef(thiz);
}

void prepare(JNIEnv *env, jobject thiz, jstring source) {
    if (IS_TEST)
        return;

    const char *filePath = env->GetStringUTFChars(source, nullptr);
    LOGI("prepare source:%s", filePath);

    //ffmpeg init
    AVFormatContext *formatContext = avformat_alloc_context();
    avformat_open_input(&formatContext, filePath, nullptr, nullptr);
    avformat_find_stream_info(formatContext, nullptr);
    uint streamsCount = formatContext->nb_streams;
    LOGI("file duration:%ld us", formatContext->duration);
    LOGI("streams count : %d", streamsCount);
    int audioStreamIndex = -1;
    for (int i = 0; i < streamsCount; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }
    AVCodecParameters *codecParameters = formatContext->streams[audioStreamIndex]->codecpar;
    LOGI("audioStreamIndex:%d", audioStreamIndex);
    const AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecParameters);

    //ffAudio use Opensl es play pcm
    ffAudio = new FFAudio(formatContext, codecContext, codecParameters, audioStreamIndex,callJava);
    LOGI("codecContext %d,%d,%d,%d", codecContext->time_base.num, codecContext->time_base.den,
         codecContext->pkt_timebase.num, codecContext->pkt_timebase.den);
    ffAudio->prepare();
    // OpenSL ES准备好了后就可以从其配置好的回调接口
    // 先去读packet放入queue中，然后主动调用解码packet
    ffAudio->start();

    env->ReleaseStringUTFChars(source, filePath);
}


/***********************************************************************************************************
 * register native methods
 * ********************************************************************************************************/
static int
registerNativeMethods(JNIEnv *envVar, const char *inClassName, JNINativeMethod *inMethodsList,
                      int inNumMethods);

int registerNatives(JNIEnv *env);

typedef union {
    JNIEnv *env;
    void *venv;
} UnionJNIEnvToVoid;

/*
* This is called by the VM when the shared library is first loaded.
* */
JNIEXPORT jint
JNI_OnLoad(JavaVM *vm, void *reserved) {

    callJava = new CallJava();
    callJava->javaVm = vm;

    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    JNIEnv *env = NULL;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        return -1;
    }
    LOGI("JNI_OnLoad Register Natives Methods Success!!!");
    return JNI_VERSION_1_4;
}

/*
 * Class and package name
 * static const char *classPathName = "com/qualcomm/qti/usta/core/AccelCalibrationJNI";
 * */
//com.mingzz__.h26x.rtmp.RTMPActivity
const char *classPathName_1 = "com/ming/ffmpeg/FFAudioActivity";

/*
 * List of native methods
 *  {"startCalibration" , "()V", (void *)startCalibration},
 *  {"getAccelCalFromNative" , "()Ljava/lang/String;", (void *)getAccelCalFromNative},
 * {"stopCalibration" , "()V", (void *)stopCalibration},
 * */
JNINativeMethod methods_1[] = {
        {"jniNativeTest",         "()V",                   (void *) test},
        {"initJNINativeCallJava", "()V",                   (void *) initCallJava},
        {"prepare",               "(Ljava/lang/String;)V", (void *) prepare},
};

/*
 * Register several native methods for one class.
 *
 *
 * */
int registerNativeMethods(JNIEnv *envVar, const char *inClassName, JNINativeMethod *inMethodsList,
                          int inNumMethods) {
    jclass javaClazz = envVar->FindClass(inClassName);
    if (javaClazz == NULL) {
        return JNI_FALSE;
    }
    if (envVar->RegisterNatives(javaClazz, inMethodsList, inNumMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
   * Register native methods for all classes we know about.
   *
   * Returns JNI_TRUE on success
   *
   * */
int registerNatives(JNIEnv *env) {
    if (!registerNativeMethods(env, classPathName_1, methods_1,
                               sizeof(methods_1) / sizeof(methods_1[0]))) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

