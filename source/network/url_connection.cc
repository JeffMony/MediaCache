//
// Created by jefflee on 2023/8/31.
//

#include "url_connection.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event.h"
#include "event2/keyvalq_struct.h"
#include "event2/bufferevent_ssl.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "response_info.h"
#include "log.h"

namespace net {

static int cert_verify_callback(X509_STORE_CTX *x509_ctx, void *arg);

static void http_request_done_callback(struct evhttp_request *req, void *ctx);

URLConnection::URLConnection()
    : proxy_bev_(NULL)
    , event_base_(NULL)
    , evhttp_connection_(NULL)
    , evhttp_uri_(NULL)
    , time_out_(15)
    , retry_count_(1)
    , ip_type_(IP_V4) {
}

URLConnection::~URLConnection() {
  LOGI("enter: %s %s %d", __FILE_NAME__, __func__ , __LINE__);
  if (evhttp_connection_ != NULL) {
    evhttp_connection_free(evhttp_connection_);
    evhttp_connection_ = NULL;
  }
  if (evhttp_uri_ != NULL) {
    evhttp_uri_free(evhttp_uri_);
    evhttp_uri_ = NULL;
  }
  if (event_base_ != NULL) {
    event_base_free(event_base_);
    event_base_ = NULL;
  }
  LOGI("leave: %s %s %d", __FILE_NAME__, __func__ , __LINE__);
}

void URLConnection::SetProxyBufferEvent(struct bufferevent *bev) {
  proxy_bev_ = bev;
}

struct bufferevent * URLConnection::GetProxyBufferEvent() {
  return proxy_bev_;
}

void URLConnection::SetTimeOut(int time_out) {
  LOGI("%s %s %d time_out=%d", __FILE_NAME__, __func__ , __LINE__, time_out);
  time_out_ = time_out;
}

void URLConnection::SetRetryCount(int retry_count) {
  LOGI("%s %s %d retry_count=%d", __FILE_NAME__, __func__ , __LINE__, retry_count);
  retry_count_ = retry_count;
}

void URLConnection::SetIpType(IP_TYPE ip_type) {
  LOGI("%s %s %d ip_type=%d", __FILE_NAME__, __func__ , __LINE__, ip_type);
  ip_type_ = ip_type;
}

int URLConnection::Start(RequestInfo *request_info) {
  auto url = request_info->url.c_str();
  if (url == NULL || strcmp(url, "") == 0) {
    return -1;
  }
  LOGI("enter: %s %s %d", __FILE_NAME__, __func__ , __LINE__);
  if (evhttp_uri_) {
    evhttp_uri_free(evhttp_uri_);
    evhttp_uri_ = NULL;
  }
  if (evhttp_connection_) {
    evhttp_connection_free(evhttp_connection_);
    evhttp_connection_ = NULL;
  }
  evhttp_uri_ = evhttp_uri_parse(url);
  if (evhttp_uri_ == NULL) {
    return -2;
  }
  const char *schema = evhttp_uri_get_scheme(evhttp_uri_);
  if (schema == NULL || (strcasecmp(schema, "http") != 0 && strcasecmp(schema, "https") != 0)) {
    return -3;
  }
  bool is_https = strcasecmp(schema, "https") == 0;
  const char *host = evhttp_uri_get_host(evhttp_uri_);
  if (host == nullptr) {
    return -4;
  }
  int port = evhttp_uri_get_port(evhttp_uri_);
  if (port == -1) {
    port = is_https ? 443 : 80;
  }
  const char *path = evhttp_uri_get_path(evhttp_uri_);
  if (strlen(path) == 0) {
    path = "/";
  }
  const char *query = evhttp_uri_get_query(evhttp_uri_);
  char uri[256];
  if (query == NULL) {
    snprintf(uri, sizeof(uri) - 1, "%s", path);
  } else {
    snprintf(uri, sizeof(uri) - 1, "%s?%s", path, query);
  }
  uri[sizeof(uri) - 1] = '\0';
  if (event_base_ == NULL) {
    event_base_ = event_base_new();
    if (event_base_ == NULL) {
      return -5;
    }
  } else {
    event_base_loopbreak(event_base_);
  }
  struct bufferevent *bev = NULL;
  SSL_CTX *ssl_ctx = NULL;
  SSL *ssl = NULL;
  if (is_https) {
    SSL_library_init();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    ssl_ctx = SSL_CTX_new(SSLv23_method());
    if (ssl_ctx == NULL) {
      return -6;
    }
    X509_STORE *store = SSL_CTX_get_cert_store(ssl_ctx);
    if (X509_STORE_set_default_paths(store) != 1) {
      return -7;
    }
    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_cert_verify_callback(ssl_ctx, cert_verify_callback, (void *) host);

    ssl = SSL_new(ssl_ctx);
    if (ssl == NULL) {
      return -8;
    }
    SSL_set_tlsext_host_name(ssl, host);
    bev = bufferevent_openssl_socket_new(event_base_, -1, ssl, BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
    bufferevent_openssl_set_allow_dirty_shutdown(bev, 1);
  } else {
    bev = bufferevent_socket_new(event_base_, -1, BEV_OPT_CLOSE_ON_FREE);
  }
  if (bev == NULL) {
    return -9;
  }

  evhttp_connection_ = evhttp_connection_base_bufferevent_new(event_base_, NULL, bev, host, port);
  if (evhttp_connection_ == NULL) {
    return -10;
  }
  if (ip_type_ == IP_V4) {
    evhttp_connection_set_family(evhttp_connection_, AF_INET);
  } else if (ip_type_ == IP_V6) {
    evhttp_connection_set_family(evhttp_connection_, AF_INET6);
  }
  if (retry_count_ > 0) {
    evhttp_connection_set_retries(evhttp_connection_, retry_count_);
  }
  if (time_out_ > 0) {
    evhttp_connection_set_timeout(evhttp_connection_, time_out_);
  }
  struct evhttp_request *request = evhttp_request_new(http_request_done_callback, this);
  if (request == NULL) {
    return -11;
  }
  struct evkeyvalq *output_headers = evhttp_request_get_output_headers(request);
  evhttp_add_header(output_headers, "Host", host);
  evhttp_add_header(output_headers, "Connection", "close");
  if (request_info != NULL) {
    std::string range_header;
    range_header.append("bytes=").append(std::to_string(request_info->range_start)).append("-").append((request_info->range_end == LONG_MAX) ? "" : std::to_string(request_info->range_end));
    evhttp_add_header(output_headers, "Range", range_header.c_str());
  }
  int ret = evhttp_make_request(evhttp_connection_, request, EVHTTP_REQ_GET, uri);
  if (ret != 0) {
    return -12;
  }
  event_base_dispatch(event_base_);
  LOGI("leave: %s %s %d", __FILE_NAME__, __func__ , __LINE__);
  return 0;
}

void URLConnection::Close() {
  if (event_base_ != NULL) {
    event_base_loopbreak(event_base_);
  }
}

/**
 * http 请求结果回调
 *
 * 可以通过判断response_code 判断接下来的逻辑如何走
 * @param req
 * @param ctx
 */
static void http_request_done_callback(struct evhttp_request *req, void *ctx) {
  if (req == NULL) {
    return;
  }
  net::URLConnection *url_connection = reinterpret_cast<net::URLConnection *>(ctx);
  struct bufferevent *proxy_bev = url_connection->GetProxyBufferEvent();
  int response_code = evhttp_request_get_response_code(req);
  struct evkeyvalq *input_headers = evhttp_request_get_input_headers(req);
  long content_length = 0;
  std::string content_type;
  std::string range;
  for (struct evkeyval *header = input_headers->tqh_first; header; header = header->next.tqe_next) {
    if (strcasecmp(header->key, "Content-Length") == 0) {
      content_length = atol(header->value);
    } else if (strcasecmp(header->key, "Content-Type") == 0) {
      content_type.append(header->value);
    } else if (strcasecmp(header->key, "Content-Range") == 0) {
      range.append(header->value);
    }
  }
  struct ResponseHeader *response_header = new ResponseHeader();
  response_header->partial = (response_code == RESPONSE_PARTIAL);
  response_header->mime_type.clear();
  response_header->mime_type.append(content_type);
  response_header->content_length = content_length;
  if (!range.empty()) {
    response_header->is_ranged = true;
    response_header->range_str.clear();
    response_header->range_str.append(range);
  }
  if (response_code == RESPONSE_OK || response_code == RESPONSE_PARTIAL) {
    std::string response_header_str = GetResponseHeader(response_header);
    bufferevent_write(proxy_bev, response_header_str.c_str(), response_header_str.length());
    char buffer[4096];
    int read;
    FILE *fp;
    fp = fopen("/sdcard/Pictures/test_ltp.mp4", "w");
    while ((read = evbuffer_remove(evhttp_request_get_input_buffer(req), buffer, sizeof(buffer))) > 0) {
      bufferevent_write(proxy_bev, buffer, read);
      fwrite(buffer, read, 1, fp);
    }
    fclose(fp);
  } else if (response_code == LOCATION_PERMANENTLY || response_code == LOCATION_TEMPORARY) {

  }
  delete response_header;
}

/**
 * 证书校验回调
 * @param x509_ctx
 * @param arg
 * @return
 */
static int cert_verify_callback(X509_STORE_CTX *x509_ctx, void *arg) {
  LOGI("%s %s %d", __FILE_NAME__, __func__ , __LINE__);
  return 1;
}

}