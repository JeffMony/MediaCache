//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_INFO_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_INFO_H_

#include <string>

namespace net {

struct RequestInfo {
  std::string url;
  long range_start;
  long range_end;
};

enum IP_TYPE {
  IP_V4 = 1000,
  IP_V6
};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_INFO_H_
