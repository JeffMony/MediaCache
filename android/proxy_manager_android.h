//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_ANDROID_PROXY_MANAGER_ANDROID_H_
#define PLAYERSDK_MEDIACACHE_ANDROID_PROXY_MANAGER_ANDROID_H_

#include <jni.h>
#include <string>
#include "proxy_server.h"

namespace proxy {

class ProxyManagerAndroid {
 public:
  explicit ProxyManagerAndroid(jobject object);

  virtual ~ProxyManagerAndroid();

  void SetCacheDir(const char *cache_dir);

  void Start();

  std::string GetProxyUrl(const char *url);

  void Close();

 private:
  jobject proxy_manager_object_;
  ProxyServer *proxy_server_;
};

}

#endif //PLAYERSDK_MEDIACACHE_ANDROID_PROXY_MANAGER_ANDROID_H_
