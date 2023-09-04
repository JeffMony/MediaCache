//
// Created by jefflee on 2023/9/3.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_PROXY_CACHE_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_PROXY_CACHE_H_

#include "request_info.h"
#include "event2/bufferevent.h"
#include "url_connection.h"
#include "buffer_listener.h"

namespace proxy {

class ProxyCache : public BufferListener {
 public:
  explicit ProxyCache();

  virtual ~ProxyCache();

  void SetProxyBufferEvent(struct bufferevent *bev);

  int Start(net::RequestInfo *request_info);

 protected:
  void OnBufferCallback(char *buffer, int buffer_length, bool header) override;

 private:
  struct bufferevent *proxy_bev_;
  net::URLConnection *url_connection_;

};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_PROXY_CACHE_H_
