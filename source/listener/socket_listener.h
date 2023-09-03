//
// Created by jefflee on 2023/9/3.
//

#ifndef PLAYERSDK_MEDIACACHE_SOURCE_LISTENER_SOCKET_LISTENER_H_
#define PLAYERSDK_MEDIACACHE_SOURCE_LISTENER_SOCKET_LISTENER_H_

namespace proxy {

class SocketListener {
 public:
  virtual void OnSocketPortCallback(int port) = 0;

  virtual void OnSocketCreateResult(bool success) = 0;
};

}
#endif //PLAYERSDK_MEDIACACHE_SOURCE_LISTENER_SOCKET_LISTENER_H_
