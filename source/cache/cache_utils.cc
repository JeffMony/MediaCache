//
// Created by jefflee on 2023/9/1.
//

#include "cache_utils.h"

namespace cache {

static std::string cache_dir_;

void CacheUtils::SetCacheDir(std::string dir) {
  cache_dir_.clear();
  cache_dir_.append(dir);
}

std::string CacheUtils::GetCacheDir() {
  return cache_dir_;
}

}