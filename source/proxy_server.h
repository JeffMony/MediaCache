//
// Created by jefflee on 2023/8/31.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_PROXY_SERVER_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_PROXY_SERVER_H_

#include <pthread.h>
#include <string>
#include "event2/http.h"

namespace proxy {

class ProxyServer {
 public:
  ProxyServer();

  virtual ~ProxyServer();

  void SetCacheDir(const char *dir);

  void Start();

  void Close();

  void StartProxyTask();

 private:
  pthread_t socket_thread_;
  std::string cache_dir_;
  struct event_base *event_base_;
  struct evconnlistener *event_conn_listener_;
  bool event_loop_running_;

};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_PROXY_SERVER_H_
