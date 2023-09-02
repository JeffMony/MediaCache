//
// Created by jefflee on 2023/8/31.
//

#include "proxy_server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include "event2/buffer.h"
#include "event2/dns.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/listener.h"
#include "compat/sys/queue.h"
#include "cache_utils.h"
#include "url_connection.h"
#include "request_parser.h"
#include "common.h"
#include "log.h"

namespace proxy {

static void *StartSocketThread(void *user_data);

static void listener_callback(
    struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socket_len, void *);
static void conn_read_callback(struct bufferevent *, void *);
static void conn_event_callback(struct bufferevent *, short, void *);

ProxyServer::ProxyServer()
    : socket_thread_()
    , cache_dir_()
    , event_base_(nullptr)
    , event_conn_listener_(nullptr)
    , event_loop_running_(false) {
}

ProxyServer::~ProxyServer() {
  pthread_join(socket_thread_, NULL);
  if (event_conn_listener_ != NULL) {
    evconnlistener_free(event_conn_listener_);
    event_conn_listener_ = NULL;
  }
  if (event_base_ != NULL) {
    event_base_free(event_base_);
    event_base_ = NULL;
  }
}

static void *StartSocketThread(void *user_data) {
  auto proxy_server = reinterpret_cast<proxy::ProxyServer *>(user_data);
  proxy_server->StartProxyTask();
  pthread_exit(NULL);
}

void ProxyServer::SetCacheDir(const char *dir) {
  cache::CacheUtils::SetCacheDir(dir);
}

void ProxyServer::Start() {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&socket_thread_, &attr, StartSocketThread, this);
}

void ProxyServer::Close() {
  if (event_base_ != NULL) {
    while (event_loop_running_) {
      event_base_loopbreak(event_base_);
    }
  }
  if (event_conn_listener_ != NULL) {
    evconnlistener_free(event_conn_listener_);
    event_conn_listener_ = NULL;
  }
  if (event_base_ != NULL) {
    event_base_free(event_base_);
    event_base_ = NULL;
  }
}

void ProxyServer::StartProxyTask() {
  if (event_base_ == NULL) {
    event_base_ = event_base_new();
    if (event_base_ == NULL) {
      LOGI("%s %s %d Could not initialize event_base", __FILE_NAME__, __func__ , __LINE__);
      return;
    }
    LOGI("%s %s %d initialize event_base success", __FILE_NAME__, __func__ , __LINE__);
  } else {
    event_base_loopbreak(event_base_);
  }
  if (event_conn_listener_ == NULL) {
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(LOCAL_PROXY);
    sin.sin_port = htons(LISTEN_PORT); /// 固定一个端口号

    event_conn_listener_ = evconnlistener_new_bind(
        event_base_, listener_callback, (void *) event_base_,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr *) &sin, sizeof(sin));

    if (event_conn_listener_ == NULL) {
      LOGI("%s %s %d Could not initialize event base listener", __FILE_NAME__, __func__ , __LINE__);
      return;
    }
    LOGI("%s %s %d initialize event base listener success", __FILE_NAME__, __func__ , __LINE__);
  }
  LOGI("%s %s %d", __FILE_NAME__, __func__ , __LINE__);
  event_loop_running_ = true;
  event_base_dispatch(event_base_);
  event_loop_running_ = false;
}

static void listener_callback(
    struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socket_len, void *user_data) {
  LOGI("%s %s %d", __FILE_NAME__, __func__ , __LINE__);
  struct event_base *base = evconnlistener_get_base(listener);
  struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
  /// 绑定读事件回调函数、写事件回调函数、错误事件回调函数
  bufferevent_setcb(bev, conn_read_callback, NULL, conn_event_callback, base);
  bufferevent_enable(bev, EV_READ | EV_WRITE);
}

static void conn_read_callback(struct bufferevent *bev, void *user_data) {
  LOGI("enter: %s %s %d bev=%p", __FILE_NAME__, __func__ , __LINE__, bev);
  struct evbuffer *input_buffer = bufferevent_get_input(bev);
  int length = evbuffer_get_length(input_buffer);
  const char *buffer_body = (const char *) evbuffer_pullup(input_buffer, length);
  net::RequestParser *request_parser = new net::RequestParser(buffer_body);
  std::string url = request_parser->GetUrl();
  LOGI("%s %s %d url=%s, %s", __FILE_NAME__, __func__ , __LINE__, url.c_str(), buffer_body);
  if (!url.empty()) {
    net::URLConnection *url_connection = new net::URLConnection();
    url_connection->SetProxyBufferEvent(bev);
    int ret = url_connection->Start(url.c_str(), request_parser->GetRequestInfo());
    LOGI("%s %s %d ret=%d", __FILE_NAME__, __func__ , __LINE__, ret);
  }
  delete request_parser;
}

static void conn_event_callback(struct bufferevent *bev, short events, void *user_data) {
  if (BEV_EVENT_CONNECTED == events) {
    LOGI("%s %s %d connect finish", __FILE_NAME__, __func__ , __LINE__);
  }
}

}