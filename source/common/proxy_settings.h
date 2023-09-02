//
// Created by jefflee on 2023/9/3.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_COMMON_PROXY_SETTINGS_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_COMMON_PROXY_SETTINGS_H_

#include "cache_config.h"

namespace common {

class ProxySettings {
 public:
  static void InitCacheConfig(cache::CacheConfig *cache_config);

  static cache::CacheConfig *GetCacheConfig();
};

}
#endif //PLAYERSDK_MEDIACACHE_SOURCE_COMMON_PROXY_SETTINGS_H_
