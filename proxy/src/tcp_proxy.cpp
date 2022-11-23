#include "tcp_proxy.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

TcpProxy::TcpProxy(/* args */) {
  host_port_ = 6101;
  proxy_port_ = 6100;
  std::string host_addres_ = "127.0.0.1";
}

TcpProxy::~TcpProxy() {}

bool TcpProxy::init() {
  // config connect to host (as client)
  struct sockaddr_in host_socket_address_in;
  memset(&host_socket_address_in, 0, sizeof(host_socket_address_in));
  host_socket_address_in.sin_family = AF_INET;
  host_socket_address_in.sin_port = htons(host_port_);
  inet_pton(AF_INET, host_addres_.c_str(), &host_socket_address_in.sin_addr);
  memcpy(&host_socket_address_, &host_socket_address_in,
         sizeof(host_socket_address_in));

  // config socket for proxy
  proxy_socket_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
  if (proxy_socket_fd_ == -1) {
    std::cerr << "Can not create soket" << std::endl;
    return false;
  }
  std::cout << "proxy_socket_fd: " << proxy_socket_fd_ << std::endl;

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

  // FD_ZERO(proxy_fd_set_);
  // FD_ZERO(client_fd_set_);
  // FD_SET(proxy_socket_fd_, proxy_fd_set_)

  return true;
}

void TcpProxy::run() {
  for (;;) {
    int connect_fd = accept(proxy_socket_fd_, NULL, NULL);
    if (connect_fd == -1) {
      std::cerr << "Accept failed" << std::endl;
      close(proxy_socket_fd_);
      continue;
    }
    std::cout << "connect_fd: " << connect_fd << std::endl;
    int size = recv(connect_fd, buffer_, sizeof buffer_, 0);
    if (size > 0) {
      {
        auto host_socket_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
        if (host_socket_fd_ == -1) {
          std::cerr << "Can not create host soket" << std::endl;
          continue;
        }

        if (connect(host_socket_fd_, &host_socket_address_,
                    sizeof(host_socket_address_)) == 0) {
          std::cout << "host_socket_fd_: " << host_socket_fd_ << std::endl;
          send(host_socket_fd_, buffer_, size, 0);
          std::cout << "SENDED" << std::endl;
        } else {
          close(host_socket_fd_);
          continue;
        }
        close(host_socket_fd_);
      }

      log({buffer_}); // log recive message
    } else {
      std::cout << "size: " << size << std::endl;
    }

    if (shutdown(connect_fd, SHUT_RDWR) == -1) {
      std::cerr << "Shutdown failed" << std::endl;
      close(connect_fd);
      close(proxy_socket_fd_);
      return;
    }
    close(connect_fd);
  }
  close(proxy_socket_fd_);
}

void TcpProxy::log(const std::string &data) {
  std::cout << "message: " << data << std::endl;
}