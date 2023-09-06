//
// Created by jefflee on 2023/9/3.
//

#include "proxy_cache.h"
#include "log.h"

namespace proxy {

ProxyCache::ProxyCache()
  : proxy_bev_(NULL)
  , url_connection_(NULL) {

}

ProxyCache::~ProxyCache() {
  if (url_connection_ != NULL) {
    delete url_connection_;
    url_connection_ = NULL;
  }
}

void ProxyCache::SetProxyBufferEvent(struct bufferevent *bev) {
  proxy_bev_ = bev;
}

int ProxyCache::Start(net::RequestInfo *request_info) {
  if (url_connection_ == NULL ) {
    url_connection_ = new net::URLConnection(this);
  }
  int ret = url_connection_->Start(request_info);
  return ret;
}

void ProxyCache::OnBufferCallback(char *buffer, int buffer_length, bool header) {
  if (proxy_bev_ != NULL) {
    bufferevent_write(proxy_bev_, buffer, buffer_length);
  }
}

}