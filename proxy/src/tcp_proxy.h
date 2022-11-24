#pragma once

#include "proxy.h"
#include <fstream>
#include <queue>
#include <set>
#include <string>
#include <sys/socket.h>
#include <unordered_map>

class TcpProxy : private Proxy {
public:
  TcpProxy();
  ~TcpProxy();
  virtual bool init() override;
  virtual void run() override;

  void log(const std::string &data);

private:
  void testConnectToHost();
  void newConnection(int fd);
  void existConnection(int fd);
  void transmitMessage(int fd, char *message, int length);
  void closeConnection(int fd);
  void shutdownSocket(int fd);

  int host_port_, proxy_port_, max_proxy_connections_ = 128;
  std::string host_addres_;
  struct sockaddr host_socket_address_;
  int proxy_socket_fd_ = -1;
  char buffer_[1024];
  fd_set proxy_fd_set_, client_fd_set_;
  std::set<int> set_client_fd_;
  struct timeval time_value_;
  std::unordered_map<int, int> map_;
  std::ofstream ofstream_;
};
