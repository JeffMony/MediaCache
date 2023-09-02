//
// Created by jefflee on 2023/9/1.
//

#include "request_parser.h"
#include "b64.h"
#include <regex>
#include "log.h"
#include "string_utils.h"

namespace net {

RequestParser::RequestParser(const char *data)
  : request_info_(nullptr) {
  Parse(data);
}

RequestParser::~RequestParser() {
  if (request_info_) {
    delete request_info_;
    request_info_ = nullptr;
  }
}

std::string RequestParser::GetUrl() {
  if (request_info_) {
    return request_info_->url;
  }
  return "";
}

RequestInfo * RequestParser::GetRequestInfo() {
  return request_info_;
}

void RequestParser::Parse(const char *data) {
  request_info_ = new RequestInfo();
  request_info_->url.clear();
  request_info_->range_start = 0;
  request_info_->range_end = LONG_MAX;
  std::regex r = std::regex("GET /(.*) HTTP");
  std::smatch m;
  std::string origin;
  origin.append(data);
  if (std::regex_search(origin.cbegin(), origin.cend(), m, r)) {
    std::string m_str = m.str();
    int length = m_str.length();
    std::string result = m_str.substr(5, length - 10);
    unsigned char *decode_str = b64_decode(result.c_str(), result.size());
    request_info_->url.append(reinterpret_cast<const char *>(decode_str));
  } else {
    r = std::regex("HEAD /(.*) HTTP");
    if (std::regex_search(origin.cbegin(), origin.cend(), m, r)) {
      std::string m_str = m.str();
      int length = m_str.length();
      std::string result = m_str.substr(6, length - 10);
      unsigned char *decode_str = b64_decode(result.c_str(), result.size());
      request_info_->url.append(reinterpret_cast<const char *>(decode_str));
    }
  }
  r = std::regex("[R,r]ange:[ ]?bytes=(\\d*)-");
  if (std::regex_search(origin.cbegin(), origin.cend(), m, r)) {
    std::string m_str = m.str();
    std::string result = m_str.substr(13);
    std::vector<std::string> ranges = common::StringUtils::Split(result, '-');
    if (ranges.size() == 1) {
      request_info_->range_start = atoi(ranges.at(0).c_str());
      request_info_->range_end = LONG_MAX;
    } else if (ranges.size() == 2) {
      request_info_->range_start = atoi(ranges.at(0).c_str());
      request_info_->range_end = atol(ranges.at(1).c_str());
    }
  }
}

}