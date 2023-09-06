//
// Created by jefflee on 2023/8/31.
//

#include <jni.h>
#include "log.h"
#include "cache_config_android.h"
#include "proxy_manager_android.h"
#include "url_connection_android.h"

extern "C" {
#include "media_cache_env.h"
}

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#define PROXY_MANAGER "com/jeffmony/mediacache/ProxyManager"
#define URL_CONNECTION "com/jeffmony/mediacache/URLConnection"

static jlong PROXY_MANAGER_CREATE(JNIEnv *env, jobject object) {
  jobject proxy_manager_object = env->NewGlobalRef(object);
  proxy::ProxyManagerAndroid *proxy_manager_android = new proxy::ProxyManagerAndroid(proxy_manager_object);
  return reinterpret_cast<jlong>(proxy_manager_android);
}

static void PROXY_MANAGER_INIT_CONFIG(JNIEnv *env, jobject object, jlong id, jobject j_config) {
  proxy::ProxyManagerAndroid *proxy_manager_android = reinterpret_cast<proxy::ProxyManagerAndroid *>(id);
  cache::CacheConfig *cache_config = cache::GetCacheConfig(env, j_config);
  proxy_manager_android->InitCacheConfig(cache_config);
}

static jstring PROXY_MANAGER_GET_PROXY_URL(JNIEnv *env, jobject object, jlong id, jstring j_url) {
  proxy::ProxyManagerAndroid *proxy_manager_android = reinterpret_cast<proxy::ProxyManagerAndroid *>(id);
  auto url = env->GetStringUTFChars(j_url, JNI_FALSE);
  std::string proxy_url = proxy_manager_android->GetProxyUrl(url);
  env->ReleaseStringUTFChars(j_url, url);
  return env->NewStringUTF(proxy_url.c_str());
}

static void PROXY_MANAGER_ADD_CACHE_LISTENER(JNIEnv *env, jobject object, jlong id, jstring j_url, jobject j_cache_listener) {

}

static void PROXY_MANAGER_REMOVE_CACHE_LISTENER(JNIEnv *env, jobject object, jlong id, jstring j_url) {

}

static void PROXY_MANAGER_START_PROXY(JNIEnv *env, jobject object, jlong id) {
  proxy::ProxyManagerAndroid *proxy_manager_android = reinterpret_cast<proxy::ProxyManagerAndroid *>(id);
  proxy_manager_android->StartProxy();
}

static void PROXY_MANAGER_CLOSE(JNIEnv *env, jobject object, jlong id) {
  proxy::ProxyManagerAndroid *proxy_manager_android = reinterpret_cast<proxy::ProxyManagerAndroid *>(id);
  proxy_manager_android->Close();
}

static void PROXY_MANAGER_STOP(JNIEnv *env, jobject object, jlong id, jstring j_url) {
  proxy::ProxyManagerAndroid *proxy_manager_android = reinterpret_cast<proxy::ProxyManagerAndroid *>(id);
  auto url = env->GetStringUTFChars(j_url, JNI_FALSE);
  proxy_manager_android->Stop(url);
  env->ReleaseStringUTFChars(j_url, url);
}

static JNINativeMethod proxyManagerMethods[] = {
    {"createHandler", "()J", (void **) PROXY_MANAGER_CREATE },
    {"initConfig", "(JLcom/jeffmony/mediacache/ProxyConfig;)V", (void **) PROXY_MANAGER_INIT_CONFIG },
    {"getProxyUrl", "(JLjava/lang/String;)Ljava/lang/String;", (void **) PROXY_MANAGER_GET_PROXY_URL },
    {"startProxy", "(J)V", (void **) PROXY_MANAGER_START_PROXY },
    {"addCacheListener", "(JLjava/lang/String;Lcom/jeffmony/mediacache/listener/CacheListener;)V", (void **) PROXY_MANAGER_ADD_CACHE_LISTENER },
    {"removeCacheListener", "(JLjava/lang/String;)V", (void **) PROXY_MANAGER_REMOVE_CACHE_LISTENER },
    {"close", "(J)V", (void **) PROXY_MANAGER_CLOSE },
    {"stop", "(JLjava/lang/String;)V", (void **) PROXY_MANAGER_STOP },
};

static jlong URL_CONNECTION_CREATE(JNIEnv *env, jobject object) {
  return 0;
}

static void URL_CONNECTION_START(JNIEnv *env, jobject object, jlong id, jstring j_url) {

}

static void URL_CONNECTION_CLOSE(JNIEnv *env, jobject object, jlong id) {

}

static JNINativeMethod urlConnectionMethods[] = {
    {"createHandler", "()J", (void **) URL_CONNECTION_CREATE },
    {"start", "(JLjava/lang/String;)V", (void **) URL_CONNECTION_START },
    {"close", "(J)V", (void **) URL_CONNECTION_CLOSE },
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  JNIEnv *env = nullptr;
  if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }
  jni_set_java_vm(vm);

  auto proxy_manager = env->FindClass(PROXY_MANAGER);
  jint proxy_manager_result = env->RegisterNatives(proxy_manager, proxyManagerMethods, NELEM(proxyManagerMethods));
  LOGI("proxy_manager_result=%d", (proxy_manager_result == JNI_OK));
  env->DeleteLocalRef(proxy_manager);

  auto url_connection = env->FindClass(URL_CONNECTION);
  jint url_connection_result = env->RegisterNatives(url_connection, urlConnectionMethods, NELEM(urlConnectionMethods));
  LOGI("url_connection_result=%d", (url_connection_result == JNI_OK));
  env->DeleteLocalRef(url_connection);

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
  JNIEnv *env = nullptr;
  if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
    return;
  }
}

#ifdef __cplusplus
}
#endif


