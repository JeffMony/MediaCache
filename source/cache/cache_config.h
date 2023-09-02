//
// Created by jefflee on 2023/9/2.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_COMMON_PROXY_CONFIG_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_COMMON_PROXY_CONFIG_H_

#include <string>

namespace cache {

enum CacheStrategy {
  LRU = 100,
  NONE
};

typedef struct CacheConfig {
  std::string cache_dir;
  long max_size;
  long expire_time;
  CacheStrategy strategy;
} CacheConfig;

static void CloneCacheConfig(CacheConfig *src, CacheConfig *dest) {
  dest->cache_dir.clear();
  dest->cache_dir.append(src->cache_dir);
  dest->max_size = src->max_size;
  dest->expire_time = src->expire_time;
  dest->strategy = src->strategy;
}

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_COMMON_PROXY_CONFIG_H_
