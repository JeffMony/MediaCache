//
// Created by jefflee on 2023/9/1.
//

#include "string_utils.h"
#include <sstream>

namespace common {

std::vector<std::string> StringUtils::Split(const std::string &s, const char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(s);
  while (std::getline(token_stream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

}