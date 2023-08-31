//
// Created by jefflee on 2023/9/1.
//

#include "proxy_manager_android.h"
#include "base64/b64.h"
#include "common.h"

namespace proxy {

ProxyManagerAndroid::ProxyManagerAndroid() {

}

ProxyManagerAndroid::~ProxyManagerAndroid() {

}

void ProxyManagerAndroid::SetCacheDir(const char *cache_dir) {

}

void ProxyManagerAndroid::Start() {

}

std::string ProxyManagerAndroid::GetProxyUrl(const char *url) {
  std::string result;
  result.append(HTTP_PROTOCOL);
  result.append(LOCAL_PROXY);
  result.append(":");
  result.append(std::to_string(LISTEN_PORT));
  result.append("/");
  int length = strlen(url);
  result.append(b64_encode(reinterpret_cast<const unsigned char *>(url), length));
  return result;
}

void ProxyManagerAndroid::Close() {

}

}