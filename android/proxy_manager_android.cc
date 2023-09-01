//
// Created by jefflee on 2023/9/1.
//

#include "proxy_manager_android.h"
#include "base64/b64.h"
#include "common.h"

extern "C" {
#include "media_cache_env.h"
}

namespace proxy {

ProxyManagerAndroid::ProxyManagerAndroid(jobject object)
  : proxy_manager_object_(object)
  , proxy_server_(NULL) {
  proxy_server_ = new proxy::ProxyServer();
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

void ProxyManagerAndroid::SetCacheDir(const char *cache_dir) {
  if (proxy_server_ != NULL) {
    proxy_server_->SetCacheDir(cache_dir);
  }
}

void ProxyManagerAndroid::Start() {
  if (proxy_server_ != NULL) {
    proxy_server_->Start();
  }
}

std::string ProxyManagerAndroid::GetProxyUrl(const char *url) {
  std::string result;
  result.append(HTTP_PROTOCOL);
  result.append(LOCAL_PROXY);
  result.append(":");
  result.append(std::to_string(LISTEN_PORT));
  result.append("/");
  int length = strlen(url);
  result.append(b64_encode(reinterpret_cast<const unsigned char *>(url), length));
  return result;
}

void ProxyManagerAndroid::Close() {
  if (proxy_server_ != NULL) {
    proxy_server_->Close();
  }
}

}