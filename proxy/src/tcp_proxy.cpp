#include "tcp_proxy.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

TcpProxy::TcpProxy() {
  proxy_port_ = 6100;
  if (const char *env_p = std::getenv("PROXY_PORT"))
    host_port_ = atoi(env_p);

  host_port_ = 6101;
  if (const char *env_p = std::getenv("PORT"))
    host_port_ = atoi(env_p);

  host_addres_ = "127.0.0.1";
  if (const char *env_p = std::getenv("HOST"))
    host_addres_ = {env_p};
}

TcpProxy::~TcpProxy() {
  for (auto fd : set_client_fd_) {
    shutdownSocket(fd);
  }
}

void TcpProxy::shutdownSocket(int fd) {
  if (shutdown(fd, SHUT_RDWR) == -1) {
    std::cerr << "Shutdown fd " << fd << " failed" << std::endl;
    close(fd);
    return;
  }
  close(fd);
}

bool TcpProxy::init() {
  // config connect to host (for client)
  struct sockaddr_in host_socket_address_in;
  memset(&host_socket_address_in, 0, sizeof(host_socket_address_in));
  host_socket_address_in.sin_family = AF_INET;
  host_socket_address_in.sin_port = htons(host_port_);
  host_socket_address_in.sin_addr.s_addr = inet_network(host_addres_.c_str());
  auto value = inet_pton(AF_INET, host_addres_.c_str(),
                         &host_socket_address_in.sin_addr);
  std::cout << "value:" << value << std::endl;
  // std::cout << "Host address: " << host_addres_ << std::endl;
  memcpy(&host_socket_address_, &host_socket_address_in,
         sizeof(host_socket_address_in));

  // config socket for proxy
  proxy_socket_fd_ = socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
  if (proxy_socket_fd_ == -1) {
    std::cerr << "Can't create proxy soket" << std::endl;
    return false;
  }
  std::cout << "Proxy socket fd: " << proxy_socket_fd_ << std::endl;

  struct sockaddr_in proxy_socket_address_in;
  memset(&proxy_socket_address_in, 0, sizeof(proxy_socket_address_in));
  proxy_socket_address_in.sin_family = AF_INET;
  proxy_socket_address_in.sin_port = htons(proxy_port_);
  proxy_socket_address_in.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind proxy socket
  if (bind(proxy_socket_fd_, (struct sockaddr *)&proxy_socket_address_in,
           sizeof(proxy_socket_address_in)) == -1) {
    std::cerr << "Bind failed" << std::endl;
    close(proxy_socket_fd_);
    return false;
  }

  if (listen(proxy_socket_fd_, max_proxy_connections_) == -1) {
    std::cerr << "Listen failed" << std::endl;
    close(proxy_socket_fd_);
    return false;
  }

  // define set
  FD_ZERO(&proxy_fd_set_);
  FD_ZERO(&client_fd_set_);
  FD_SET(proxy_socket_fd_, &proxy_fd_set_);
  set_client_fd_.insert(proxy_socket_fd_);

  return true;
}

void TcpProxy::testConnectToHost() {
  int size = 10;
  auto host_socket_fd_ =
      socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
  if (host_socket_fd_ == -1) {
    std::cerr << "Can not create host soket" << std::endl;
  }

  if (connect(host_socket_fd_, &host_socket_address_,
              sizeof(host_socket_address_)) == 0) {
    std::cout << "Host socket fd: " << host_socket_fd_ << std::endl;
    send(host_socket_fd_, buffer_, size, 0);
    std::cout << "SENDED message to host" << std::endl;
  } else {
    std::cout << "Con't connect to host" << std::endl;
    close(host_socket_fd_);
  }
  close(host_socket_fd_);
}

void TcpProxy::newConnection(int fd) {
  struct sockaddr_storage client_socket_address;
  socklen_t client_socket_address_len = sizeof client_socket_address;
  auto new_fd =
      accept(proxy_socket_fd_, (struct sockaddr *)&client_socket_address,
             &client_socket_address_len);
  if (new_fd == -1) {
    std::cerr << "Accept failed" << std::endl;
    return;
  }
  auto host_socket_fd =
      socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
  if (host_socket_fd == -1) {
    std::cerr << "Can't create host socket" << std::endl;
    close(new_fd);
    return;
  }

  if (connect(host_socket_fd, &host_socket_address_,
              sizeof(host_socket_address_)) == 0) {
    std::cout << "Connect to host: " << host_socket_fd << std::endl;
    FD_SET(new_fd, &proxy_fd_set_);
    FD_SET(host_socket_fd, &proxy_fd_set_);
    set_client_fd_.insert(new_fd);
    set_client_fd_.insert(host_socket_fd);
    map_.emplace(new_fd, host_socket_fd);
    map_.emplace(host_socket_fd, new_fd);
    std::cout << "Connection pair between fd " << new_fd << " and "
              << host_socket_fd << std::endl;
  } else {
    std::cerr << "Can't connect to host socket" << std::endl;
    close(host_socket_fd);
  }
}

void TcpProxy::transmitMessage(int fd, char *message, int length) {
  log({message});
  if (map_.find(fd) == map_.end()) {
    std::cerr << "Can't find socket pair" << std::endl;
    return;
  }

  if (send(map_[fd], message, length, 0) != -1) {
    std::cout << "Send data from " << fd << " to " << map_[fd] << std::endl;
  } else {
    std::cout << "Can't send data from " << fd << " to " << map_[fd]
              << std::endl;
  }
}

void TcpProxy::closeConnection(int fd) {
  shutdownSocket(fd);
  FD_CLR(fd, &proxy_fd_set_);
  set_client_fd_.erase(fd);
  map_.erase(fd);
}

void TcpProxy::existConnection(int fd) {
  int size = recv(fd, buffer_, sizeof buffer_, 0);
  if (size > 0) {
    transmitMessage(fd, buffer_, size);
  } else {
    const auto it = map_.find(fd);
    if (it == map_.end()) {
      shutdownSocket(fd);
    } else {
      auto first = it->first, second = it->second;
      closeConnection(first);
      closeConnection(second);
    }
  }
}

void TcpProxy::run() {
  for (;;) {
    time_value_ = {10, 0};
    client_fd_set_ = proxy_fd_set_;
    auto max_fd = *(set_client_fd_.rbegin()) + 1;
    auto flag = select(max_fd, &client_fd_set_, NULL, NULL, &time_value_);
    if (flag == -1) {
      throw std::runtime_error("Select failed");
    } else if (flag == 0) {
      std::cout << "Select timeout" << std::endl;
    }
    for (int fd = proxy_socket_fd_; fd < max_fd; fd++) {
      if (!FD_ISSET(fd, &client_fd_set_))
        continue;
      if (fd == proxy_socket_fd_) {
        newConnection(fd);
      } else {
        existConnection(fd);
      }
    }
  }
}

void TcpProxy::log(const std::string &data) {
  std::cout << "message: " << data << std::endl;
}