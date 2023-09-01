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

  void SetProxyBufferEvent(struct bufferevent *bev);

  struct bufferevent *GetProxyBufferEvent();

  void SetTimeOut(int time_out);

  void SetRetryCount(int retry_count);

  void SetIpType(IP_TYPE ip_type);

  int Start(const char *url, RequestInfo *request_info);

  void Close();

 private:
  struct bufferevent *proxy_bev_;
  struct event_base *event_base_;
  struct evhttp_connection *evhttp_connection_;
  struct evhttp_uri *evhttp_uri_;
  int time_out_;
  int retry_count_;
  IP_TYPE ip_type_;
};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_URL_CONNECTION_H_
