//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_CACHE_CACHE_UTILS_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_CACHE_CACHE_UTILS_H_

#include <string>

namespace cache {

class CacheUtils {
 public:
  static void SetCacheDir(std::string dir);

  static std::string GetCacheDir();

};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_CACHE_CACHE_UTILS_H_
