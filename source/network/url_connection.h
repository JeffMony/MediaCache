//
// Created by jefflee on 2023/8/31.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_URL_CONNECTION_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_URL_CONNECTION_H_

#include "event2/http.h"
#include "request_info.h"

namespace net {

class URLConnection {
 public:
  explicit URLConnection();

  virtual ~URLConnection();

  void SetSocketBufferEvent(struct bufferevent *bev);

  struct bufferevent *GetSocketBufferEvent();

  int Start(RequestInfo *request_info);

  void Close();

 private:
  struct bufferevent *socket_bev_;
  struct event_base *event_base_;
  struct evhttp_connection *evhttp_connection_;

};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_URL_CONNECTION_H_
