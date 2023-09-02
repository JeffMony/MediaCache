//
// Created by jefflee on 2023/9/3.
//

#include "proxy_settings.h"

namespace common {

static cache::CacheConfig *cache_config_ = NULL;

void ProxySettings::InitCacheConfig(cache::CacheConfig *cache_config) {
  if (cache_config_ != NULL) {
    delete cache_config_;
    cache_config_ = NULL;
  }
  cache_config_ = cache_config;
}

cache::CacheConfig *ProxySettings::GetCacheConfig() {
  return cache_config_;
}

}