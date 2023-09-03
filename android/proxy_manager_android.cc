//
// Created by jefflee on 2023/9/1.
//

#include "proxy_manager_android.h"
#include "base64/b64.h"
#include "common.h"
#include "log.h"

extern "C" {
#include "media_cache_env.h"
}

namespace proxy {

ProxyManagerAndroid::ProxyManagerAndroid(jobject object)
  : proxy_manager_object_(object)
  , proxy_manager_(NULL)
  , socket_created_(false)
  , socket_port_(0) {
  proxy_manager_ = new proxy::ProxyManager();
}

ProxyManagerAndroid::~ProxyManagerAndroid() {
  JNIEnv *env = NULL;
  int ret = jni_get_env(&env);
  if (env != NULL) {
    if (proxy_manager_object_ != NULL) {
      env->DeleteGlobalRef(proxy_manager_object_);
      proxy_manager_object_ = NULL;
    }
  }
  if (ret == JNI_EDETACHED) {
    jni_detach_thread_env();
  }
}

void ProxyManagerAndroid::InitCacheConfig(cache::CacheConfig *cache_config) {
  if (proxy_manager_ != NULL) {
    proxy_manager_->InitCacheConfig(cache_config);
  }
}

void ProxyManagerAndroid::Start() {
  if (proxy_manager_ != NULL) {
    proxy_manager_->Start(this);
  }
}

std::string ProxyManagerAndroid::GetProxyUrl(const char *url) {
  std::string result;
  result.clear();
  if (socket_created_) {
    result.append(HTTP_PROTOCOL);
    result.append(LOCAL_PROXY);
    result.append(":");
    result.append(std::to_string(socket_port_));
    result.append("/");
    int length = strlen(url);
    result.append(b64_encode(reinterpret_cast<const unsigned char *>(url), length));
  } else {
    result.append(url);
  }
  return result;
}

void ProxyManagerAndroid::Close() {
  if (proxy_manager_ != NULL) {
    proxy_manager_->Close();
  }
}

void ProxyManagerAndroid::OnSocketPortCallback(int port) {
  LOGI("%s %s %d port=%d", __FILE_NAME__, __func__ , __LINE__, port);
  socket_port_ = port;
}

void ProxyManagerAndroid::OnSocketCreateResult(bool success) {
  LOGI("%s %s %d success=%d", __FILE_NAME__, __func__ , __LINE__, success);
  socket_created_ = success;
}

}