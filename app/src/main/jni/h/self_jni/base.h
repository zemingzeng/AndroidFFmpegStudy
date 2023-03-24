//
// Created by mingzz_ on 2023/2/7.
//

#ifndef FFMPEG_BASE_H
#define FFMPEG_BASE_H

#include <jni.h>
#include <string>
#include "android/log.h"
#include <unistd.h>
#include "android/native_window_jni.h"

#define DEBUG true

#define LOGI(...) \
if (DEBUG)        \
__android_log_print(ANDROID_LOG_INFO,"mingzz-JNINative",__VA_ARGS__)

#define LOGD(...) \
if (DEBUG)        \
__android_log_print(ANDROID_LOG_DEBUG,"mingzz-JNINative",__VA_ARGS__)

#define LOGW(...) \
if(DEBUG) \
__android_log_print(ANDROID_LOG_WARN,"mingzz-JNINative",__VA_ARGS__)

#define LOGE(...) \
if(DEBUG)         \
__android_log_print(ANDROID_LOG_ERROR,"mingzz-JNINative",__VA_ARGS__)


#endif //FFMPEG_BASE_H
