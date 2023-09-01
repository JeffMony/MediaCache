//
// Created by jefflee on 2023/9/2.
//

#ifndef PLAYERSDK_MEDIACACHE_ANDROID_URL_CONNECTION_ANDROID_H_
#define PLAYERSDK_MEDIACACHE_ANDROID_URL_CONNECTION_ANDROID_H_

#include <jni.h>
#include "url_connection.h"

namespace net {

class URLConnectionAndroid {
 public:
  explicit URLConnectionAndroid(jobject object);

  virtual ~URLConnectionAndroid();

  void Start(const char *url);

  void Close();

 private:
  jobject url_connection_object_;
  URLConnection *url_connection_;
};

}

#endif //PLAYERSDK_MEDIACACHE_ANDROID_URL_CONNECTION_ANDROID_H_
