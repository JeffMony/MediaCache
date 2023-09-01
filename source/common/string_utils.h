//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_COMMON_STRING_UTILS_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_COMMON_STRING_UTILS_H_

#include <vector>
#include <string>

namespace common {

class StringUtils {
 public:
  static std::vector<std::string> Split(const std::string& s, const char delimiter);
};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_COMMON_STRING_UTILS_H_
