//
// Created by jefflee on 2023/9/1.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_RESPONSE_INFO_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_RESPONSE_INFO_H_

#include <stdio.h>
#include <string>

static const char *RESPONSE_STATUS = "HTTP/1.1 200 OK";
static const char *RESPONSE_PARTIAL_STATUS = "HTTP/1.1 206 PARTIAL CONTENT";
static const char *ACCEPT_RANGES = "Accept-Ranges: bytes";
static const char *CONTENT_LENGTH = "Content-Length: ";
static const char *CONTENT_RANGES = "Content-Range: bytes ";
static const char *CONTENT_TYPE = "Content-Type: ";

static const int RESPONSE_OK = 200;
static const int RESPONSE_PARTIAL = 206;
static const int LOCATION_PERMANENTLY = 301;
static const int LOCATION_TEMPORARY = 302;

typedef struct ResponseHeader {
  bool partial;
  std::string mime_type;
  long content_length;
  bool is_ranged;
  long range_start;
  long range_end;
  std::string range_str;
} ResponseHeader;

static std::string GetResponseHeader(ResponseHeader *response_header) {
  std::string result;
  result.append(response_header->partial ? RESPONSE_PARTIAL_STATUS : RESPONSE_STATUS);
  result.append("\r\n");
  result.append(ACCEPT_RANGES);
  result.append("\r\n");
  char msg[100];
  sprintf(msg, "Content-Length: %lld", response_header->content_length);
  result.append(msg);
  result.append("\r\n");
  if (response_header->is_ranged) {
    if (!response_header->range_str.empty()) {
      sprintf(msg, "Content-Range: %s", response_header->range_str.c_str());
    } else {
      sprintf(msg, "Content-Range: bytes %lld-%lld/%lld", response_header->range_start, response_header->range_end, response_header->content_length);
    }
    result.append(msg);
    result.append("\r\n");
  }
  sprintf(msg, "Content-Type: %s", response_header->mime_type.c_str());
  result.append(msg);
  result.append("\r\n\r\n");
  return result;
}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_NETWORK_RESPONSE_INFO_H_
