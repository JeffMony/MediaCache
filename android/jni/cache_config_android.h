//
// Created by jefflee on 2023/9/2.
//

#ifndef PLAYERSDK_MEDIACACHE_ANDROID_JNI_CACHE_CONFIG_ANDROID_H_
#define PLAYERSDK_MEDIACACHE_ANDROID_JNI_CACHE_CONFIG_ANDROID_H_

#include <jni.h>
#include <string>
#include "cache_config.h"

namespace cache {

static CacheConfig *GetCacheConfig(JNIEnv *env, jobject j_config) {
  auto clazz = env->GetObjectClass(j_config);
  jstring j_cache_dir = reinterpret_cast<jstring>(env->CallObjectMethod(j_config, env->GetMethodID(clazz, "getCacheDir", "()Ljava/lang/String;")));
  jlong max_size = env->CallLongMethod(j_config, env->GetMethodID(clazz, "getMaxSize", "()J"));
  jlong expire_time = env->CallLongMethod(j_config, env->GetMethodID(clazz, "getExpireTime", "()J"));
  jint cache_clean_strategy = env->CallIntMethod(j_config, env->GetMethodID(clazz, "getCacheCleanStrategy", "()I"));
  auto cache_dir = env->GetStringUTFChars(j_cache_dir, JNI_FALSE);
  CacheStrategy strategy = CacheStrategy::NONE;
  if (cache_clean_strategy == 0) {
    strategy = CacheStrategy::LRU;
  }
  CacheConfig cache_config = {
      .cache_dir = std::string(cache_dir),
      .max_size = max_size,
      .expire_time = expire_time,
      .strategy = strategy
  };
  CacheConfig *result_cache_config = new CacheConfig();
  CloneCacheConfig(&cache_config, result_cache_config);
  env->ReleaseStringUTFChars(j_cache_dir, cache_dir);
  env->DeleteLocalRef(clazz);
  return result_cache_config;
}

}

#endif //PLAYERSDK_MEDIACACHE_ANDROID_JNI_CACHE_CONFIG_ANDROID_H_
