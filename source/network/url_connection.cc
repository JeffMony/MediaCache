//
// Created by jefflee on 2023/8/31.
//

#include "url_connection.h"

namespace net {

URLConnection::URLConnection()
  : socket_bev_(NULL)
  , event_base_(NULL)
  , evhttp_connection_(NULL) {

}

URLConnection::~URLConnection() {

}

void URLConnection::SetSocketBufferEvent(struct bufferevent *bev) {
  socket_bev_ = bev;
}

struct bufferevent *URLConnection::GetSocketBufferEvent() {
  return socket_bev_;
}

int URLConnection::Start(RequestInfo *request_info) {
  return 0;
}

void URLConnection::Close() {

}

}