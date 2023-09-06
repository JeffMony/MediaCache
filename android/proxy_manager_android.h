//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_ANDROID_PROXY_MANAGER_ANDROID_H_
#define PLAYERSDK_MEDIACACHE_ANDROID_PROXY_MANAGER_ANDROID_H_

#include <jni.h>
#include <string>
#include "proxy_manager.h"
#include "cache_config.h"
#include "socket_listener.h"

namespace proxy {

class ProxyManagerAndroid : public SocketListener {
 public:
  explicit ProxyManagerAndroid(jobject object);

  virtual ~ProxyManagerAndroid();

  void InitCacheConfig(cache::CacheConfig *cache_config);

  void StartProxy();

  std::string GetProxyUrl(const char *url);

  void Stop(const char *url);

  void Close();

 protected:
  void OnSocketPortCallback(int port) override;

  void OnSocketCreateResult(bool success) override;

 private:
  jobject proxy_manager_object_;
  ProxyManager *proxy_manager_;
  bool socket_created_;
  int socket_port_;
};

}

#endif //PLAYERSDK_MEDIACACHE_ANDROID_PROXY_MANAGER_ANDROID_H_
