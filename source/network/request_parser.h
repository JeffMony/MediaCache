//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_PARSER_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_PARSER_H_

#include <string>
#include "request_info.h"

namespace net {

class RequestParser {
 public:
  RequestParser(const char *data);

  virtual ~RequestParser();

  std::string GetUrl();

  RequestInfo *GetRequestInfo();

 private:
  void Parse(const char *data);

 private:
  RequestInfo *request_info_;

};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_REQUEST_PARSER_H_
