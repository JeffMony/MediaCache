//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_INFO_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_INFO_H_

#include <string>

struct RequestInfo {
  std::string url;
  long range_start;
  long range_end;
};

#endif //PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_INFO_H_
