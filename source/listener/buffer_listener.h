//
// Created by jefflee on 2023/9/4.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_LISTENER_BUFFER_LISTENER_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_LISTENER_BUFFER_LISTENER_H_

namespace proxy {

class BufferListener {
 public:
  /**
   *
   * @param buffer
   * @param buffer_length
   * @param head 是否是response header
   */
  virtual void OnBufferCallback(char *buffer, int buffer_length, bool header) = 0;
};

}

#endif //PLAYERSDK_MEDIACACHE_SOURCE_LISTENER_BUFFER_LISTENER_H_
