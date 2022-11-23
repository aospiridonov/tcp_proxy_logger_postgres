#pragma once

#include "proxy.h"
#include <string>
#include <sys/socket.h>

class TcpProxy : private Proxy {
public:
  TcpProxy(/* args */);
  ~TcpProxy();
  virtual bool connect() override;
  virtual void run() override;

private:
  int host_port_, proxy_port_, max_proxy_connections_ = 10;
  std::string host_addres_;
  struct sockaddr host_socket_address_;
  int proxy_socket_fd_ = -1;
};
