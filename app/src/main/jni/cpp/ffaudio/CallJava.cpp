//
// Created by mingzz on 2023/2/22.
//
#include "CallJava.h"

JNIEnv *CallJava::callPrepare(ThreadWhere where, jmethodID *methodId,
                              bool *hasThreadAttach, const char *name,
                              const char *sign) const {
    if (nullptr == javaVm || nullptr == mainThreadEnv || nullptr == globalJavaObject) {
        LOGE("should not be like this : 0 == javaVm || 0 == mainThreadEnv || 0 == mainThreadJavaObject!");
        return nullptr;
    }
    JNIEnv *env;
    *hasThreadAttach = false;
    if (ThreadWhere::MAIN_THREAD == where) {
        env = mainThreadEnv;
    } else if (ThreadWhere::CHILD_THREAD == where) {
        jint ret = javaVm->GetEnv((void **) &env, JNI_VERSION_1_6);
        LOGI("CHILD_THREAD javaVm->GetEnv : %d", ret);
        if (JNI_EDETACHED == ret) {
            ret = javaVm->AttachCurrentThread(&env, nullptr);
            if (JNI_OK == ret) {
                *hasThreadAttach = true;
            } else {
                LOGE("CHILD_THREAD AttachCurrentThread fail : %d", ret);
                return nullptr;
            }
        }
    } else {
        LOGE("the where must be MAIN_THREAD or CHILD_THREAD!");
        return nullptr;
    }
    *methodId = env->GetMethodID(
            env->GetObjectClass(globalJavaObject),
            name, sign);
    return env;
}

void CallJava::callFinish(va_list *args, jboolean hasThreadAttach) const {
    va_end(*args);
    if (hasThreadAttach) {
        javaVm->DetachCurrentThread();
    }
}

void CallJava::CallVoidMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return;
    }
    va_list args;
    va_start(args, sign);
    env->CallVoidMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
}

jint CallJava::CallIntMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jint result = env->CallIntMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jlong CallJava::CallLongMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jlong result = env->CallLongMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jdouble CallJava::CallDoubleMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jdouble result = env->CallDoubleMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jfloat CallJava::CallFloatMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jfloat result = env->CallFloatMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jbyte CallJava::CallByteMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jbyte result = env->CallByteMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jchar CallJava::CallCharMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jchar result = env->CallCharMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jshort CallJava::CallShortMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jshort result = env->CallShortMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jboolean CallJava::CallBooleanMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return -1;
    }
    va_list args;
    va_start(args, sign);
    jboolean result = env->CallBooleanMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}

jobject CallJava::CallObjectMethodV(ThreadWhere where, const char *name, const char *sign, ...) {
    jmethodID methodId;
    bool hasThreadAttach;
    JNIEnv *env = callPrepare(where, &methodId, &hasThreadAttach, name, sign);
    if (nullptr == env) {
        LOGI("call fail env should not be null!");
        return nullptr;
    }
    va_list args;
    va_start(args, sign);
    jobject result = env->CallObjectMethodV(globalJavaObject, methodId, args);
    jboolean hasThreadAttach_ = JNI_FALSE;
    if (hasThreadAttach)
        hasThreadAttach_ = JNI_TRUE;
    callFinish(&args, hasThreadAttach_);
    return result;
}






