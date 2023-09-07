//
// Created by jefflee on 2023/8/31.
//

#include "proxy_manager.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include "event2/buffer.h"
#include "event2/dns.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/listener.h"
#include "compat/sys/queue.h"
#include "proxy_settings.h"
#include "request_parser.h"
#include "common.h"
#include "log.h"

namespace proxy {

static std::map<std::string, ProxyCache *> proxy_cache_map_;

static void *StartProxyThread(void *user_data);
static void listener_callback(
    struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socket_len, void *);
static void conn_read_callback(struct bufferevent *, void *);
static void conn_event_callback(struct bufferevent *, short, void *);

ProxyManager::ProxyManager()
  : socket_thread_()
  , socket_listener_(NULL)
  , cache_dir_()
  , event_base_(NULL)
  , event_conn_listener_(NULL)
  , event_loop_running_(false) {
}

ProxyManager::~ProxyManager() {
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

static void *StartProxyThread(void *user_data) {
  auto proxy_manager = reinterpret_cast<proxy::ProxyManager *>(user_data);
  proxy_manager->StartProxyInternal();
  pthread_exit(NULL);
}

void ProxyManager::InitCacheConfig(cache::CacheConfig *cache_config) {
  common::ProxySettings::InitCacheConfig(cache_config);
}

void ProxyManager::StartProxy(SocketListener *listener) {
  socket_listener_ = listener;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&socket_thread_, &attr, StartProxyThread, this);
}

void ProxyManager::Stop(const char *url) {
  auto iterator = proxy_cache_map_.find(std::string(url));
  if (iterator != proxy_cache_map_.end()) {
    proxy_cache_map_.erase(iterator);
    delete iterator->second;
  }
}

void ProxyManager::Close() {
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

void ProxyManager::StartProxyInternal() {
  if (event_base_ == NULL) {
    event_base_ = event_base_new();
    if (event_base_ == NULL) {
      LOGI("%s %s %d Could not initialize event_base", __FILE_NAME__, __func__ , __LINE__);
      OnSocketCreateFailed();
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
    sin.sin_port = 0;       /// 随机分配一个可用的端口号

    evutil_socket_t result_fd = 0;
    event_conn_listener_ = evconnlistener_new_bind_custom(
        event_base_, listener_callback, (void *) event_base_,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr *) &sin, sizeof(sin), &result_fd);

    if (event_conn_listener_ == NULL) {
      LOGI("%s %s %d Could not initialize event base listener", __FILE_NAME__, __func__ , __LINE__);
      OnSocketCreateFailed();
      return;
    }

    struct sockaddr_in conn_addr;
    memset(&conn_addr, 0, sizeof(struct sockaddr_in));
    unsigned int socket_length = sizeof(conn_addr);
    int ret = getsockname(result_fd, (sockaddr*)&conn_addr, &socket_length);
    if (ret != 0) {
      LOGE("%s %s %d getsockname failed ret=%d", __FILE_NAME__, __func__ , __LINE__, ret);
      OnSocketCreateFailed();
      if (event_conn_listener_ != NULL) {
        evconnlistener_free(event_conn_listener_);
        event_conn_listener_ = NULL;
      }
      return;
    }
    int result_port = ntohs(conn_addr.sin_port);
    LOGI("%s %s %d initialize event base listener success, port=%d", __FILE_NAME__, __func__ , __LINE__, result_port);
    if (socket_listener_ != NULL) {
      socket_listener_->OnSocketPortCallback(result_port);
    }
    OnSocketCreateSuccess();
  }
  LOGI("%s %s %d", __FILE_NAME__, __func__ , __LINE__);
  event_loop_running_ = true;
  event_base_dispatch(event_base_);
  event_loop_running_ = false;
}

void ProxyManager::OnSocketCreateFailed() {
  if (socket_listener_ != NULL) {
    socket_listener_->OnSocketCreateResult(false);
  }
}

void ProxyManager::OnSocketCreateSuccess() {
  if (socket_listener_ != NULL) {
    socket_listener_->OnSocketCreateResult(true);
  }
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
  auto iterator = proxy_cache_map_.find(url);
  int ret;
  if (iterator != proxy_cache_map_.end()) {
    ProxyCache *proxy_cache = iterator->second;
    ret = proxy_cache->Start(request_parser->GetRequestInfo());
  } else {
    ProxyCache *proxy_cache = new ProxyCache();
    proxy_cache->SetProxyBufferEvent(bev);
    ret = proxy_cache->Start(request_parser->GetRequestInfo());
    proxy_cache_map_.insert(std::pair<std::string, ProxyCache *>(url, proxy_cache));
  }
  delete request_parser;
  LOGI("leave: %s %s %d ret=%d", __FILE_NAME__, __func__ , __LINE__, ret);
}

static void conn_event_callback(struct bufferevent *bev, short events, void *user_data) {
  if (BEV_EVENT_CONNECTED == events) {
    LOGI("%s %s %d connect finish", __FILE_NAME__, __func__ , __LINE__);
  }
}

}