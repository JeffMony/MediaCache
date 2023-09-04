
#ifndef MD5_H_
#define MD5_H_

#include <stdint.h>
#include <string>

namespace proxy {

typedef uint32_t u32;
typedef uint8_t u8;

struct MD5Context {
  u32 buf[4];
  u32 bits[2];
  u8 in[64];
};

void MD5Init(struct MD5Context *ctx);

void MD5Update(struct MD5Context *ctx, unsigned char const *buf, size_t len);

void MD5Final(unsigned char digest[16], struct MD5Context *ctx);

void ConvertIdentifierToString(const unsigned char identifier[16],
                               char *buffer, int buffer_length);

std::string MD5(unsigned char *buf, size_t len);

}  // namespace google_breakpad

#endif  // MD5_H_