//
// Created by jefflee on 2023/8/31.
//

#ifndef PLAYERSDK_MEDIACACHE_ANDROID_ANDROID_LOG_H_
#define PLAYERSDK_MEDIACACHE_ANDROID_ANDROID_LOG_H_

#include <android/log.h>
#define TAG "media_cache"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#endif //PLAYERSDK_MEDIACACHE_ANDROID_ANDROID_LOG_H_
