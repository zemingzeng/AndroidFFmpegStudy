//
// Created by zemizeng on 2023/2/8.
//

#ifndef FFMPEG_PLAYER_H
#define FFMPEG_PLAYER_H


#include <jni.h>

class Player {
public :
    void getStreamInfo(JNIEnv *env, const char *, jobject thiz, jobject surface);

    void prepareANativeWindow(jobject);

    void prepareAudioTrack(int32_t, int32_t, int32_t);

    void destroy();

public:
    JavaVM *javaVm;
};

#endif //FFMPEG_PLAYER_H
