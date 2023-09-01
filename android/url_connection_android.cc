//
// Created by jefflee on 2023/9/2.
//

#include "url_connection_android.h"

extern "C" {
#include "media_cache_env.h"
}

namespace net {

URLConnectionAndroid::URLConnectionAndroid(jobject object)
    : url_connection_object_(object)
    , url_connection_(NULL) {

}

URLConnectionAndroid::~URLConnectionAndroid() {
  if (url_connection_ != NULL) {
    delete url_connection_;
    url_connection_ = NULL;
  }
  JNIEnv *env = NULL;
  int ret = jni_get_env(&env);
  if (env != NULL) {
    if (url_connection_object_ != NULL) {
      env->DeleteGlobalRef(url_connection_object_);
      url_connection_object_ = NULL;
    }
  }
  if (ret == JNI_EDETACHED) {
    jni_detach_thread_env();
  }
}

void URLConnectionAndroid::Start(const char *url) {
  if (url_connection_ == NULL) {
    url_connection_ = new URLConnection();
  }
  url_connection_->Start(url, NULL);
}

void URLConnectionAndroid::Close() {
  if (url_connection_ != NULL) {
    url_connection_->Close();
  }
}

}