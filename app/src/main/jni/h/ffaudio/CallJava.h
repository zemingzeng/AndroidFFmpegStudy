//
// Created by mingzz on 2023/2/22.
//

#ifndef FFMPEG_CALLJAVA_H
#define FFMPEG_CALLJAVA_H

#include "base.h"

enum class ThreadWhere {
    MAIN_THREAD,
    CHILD_THREAD
};


class CallJava {
public:
    JavaVM *javaVm;
    JNIEnv *mainThreadEnv;
    jobject globalJavaObject;

    void CallVoidMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jint CallIntMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jlong CallLongMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jdouble CallDoubleMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jfloat CallFloatMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jbyte CallByteMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jchar CallCharMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jshort CallShortMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jboolean CallBooleanMethodV(ThreadWhere where, const char *name, const char *sign, ...);

    jobject CallObjectMethodV(ThreadWhere where, const char *name, const char *sign, ...);

private:
    JNIEnv *callPrepare(ThreadWhere where, jmethodID *methodId, bool *hasThreadAttach, const char *name,
                        const char *sign) const;

    void callFinish(va_list *args, jboolean hasThreadAttach) const;
};

#endif //FFMPEG_CALLJAVA_H
