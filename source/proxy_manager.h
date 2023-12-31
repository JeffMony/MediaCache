//
// Created by jefflee on 2023/8/31.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_PROXY_MANAGER_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_PROXY_MANAGER_H_

#include <pthread.h>
#include <string>
#include <map>
#include "event2/http.h"
#include "socket_listener.h"
#include "cache_config.h"
#include "proxy_cache.h"

namespace proxy {

class ProxyManager {
 public:
  ProxyManager();

  virtual ~ProxyManager();

  void InitCacheConfig(cache::CacheConfig *cache_config);

  void StartProxy(SocketListener *listener);

  void Stop(const char *url);

  void Close();

  void StartProxyInternal();

 private:
  void OnSocketCreateFailed();

  void OnSocketCreateSuccess();

 private:
  pthread_t socket_thread_;
  SocketListener *socket_listener_;
  std::string cache_dir_;
  struct event_base *event_base_;
  struct evconnlistener *event_conn_listener_;
  bool event_loop_running_;

};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_PROXY_MANAGER_H_
