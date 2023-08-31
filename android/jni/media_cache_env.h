//
// Created by jefflee on 2023/8/31.
//

#ifndef PLAYERSDK_MEDIACACHE_ANDROID_JNI_MEDIA_CACHE_ENV_H_
#define PLAYERSDK_MEDIACACHE_ANDROID_JNI_MEDIA_CACHE_ENV_H_

#include <jni.h>

int jni_set_java_vm(void* vm);

JavaVM* jni_get_java_vm();

int jni_get_env(JNIEnv** env);

void jni_detach_thread_env();

#endif //PLAYERSDK_MEDIACACHE_ANDROID_JNI_MEDIA_CACHE_ENV_H_
