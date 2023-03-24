
#include "base.h"
#include "player.h"

jstring sayHello(JNIEnv *env, jclass clazz) {
    std::string hello = "hello FFmpeg!!!!";
    return env->NewStringUTF(hello.c_str());
}

Player player;

void play(JNIEnv *env, jobject thiz, jstring filePath, jobject surface) {

    const char *filePath_ = env->GetStringUTFChars(filePath, 0);

    player.getStreamInfo(env, filePath_, thiz, surface);

    env->ReleaseStringUTFChars(filePath, filePath_);
}

void destroy() {
    LOGI("destroy!");
    player.destroy();
}





/***********************************************************************************************************
 * register native methods
 * ********************************************************************************************************/

/*
 * Class and package name
 * static const char *classPathName = "com/qualcomm/qti/usta/core/AccelCalibrationJNI";
 * */
//com.mingzz__.h26x.rtmp.RTMPActivity
static const char *classPathName_1 = "com/ming/ffmpeg/FFmpegActivity";

/*
 * List of native methods
 *  {"startCalibration" , "()V", (void *)startCalibration},
 *  {"getAccelCalFromNative" , "()Ljava/lang/String;", (void *)getAccelCalFromNative},
 * {"stopCalibration" , "()V", (void *)stopCalibration},
 * */
static JNINativeMethod methods_1[] = {
        {"stringFromJNI", "()Ljava/lang/String;",                        (void *) sayHello},
        {"play",          "(Ljava/lang/String;Landroid/view/Surface;)V", (void *) play},
        {"destroy",       "()V",                                         (void *) destroy},
};

/*
 * Register several native methods for one class.
 *
 *
 * */
static int
registerNativeMethods(JNIEnv *envVar, const char *inClassName, JNINativeMethod *inMethodsList,
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
static int registerNatives(JNIEnv *env) {
    if (!registerNativeMethods(env, classPathName_1, methods_1,
                               sizeof(methods_1) / sizeof(methods_1[0]))) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


typedef union {
    JNIEnv *env;
    void *venv;
} UnionJNIEnvToVoid;

/*
* This is called by the VM when the shared library is first loaded.
* */
JNIEXPORT jint
JNI_OnLoad(JavaVM *vm, void *reserved) {

    player.javaVm = vm;

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
