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

bool TcpProxy::connect() {
  // config connect to host (as client)
  struct sockaddr_in host_socket_address_in;
  memset(&host_socket_address_in, 0, sizeof(host_socket_address_in));
  host_socket_address_in.sin_family = AF_INET;
  host_socket_address_in.sin_port = htons(host_port_);
  inet_pton(AF_INET, host_addres_.c_str(), &host_socket_address_in.sin_addr);
  memcpy(&host_socket_address_, &host_socket_address_in,
         sizeof(host_socket_address_in));

  // config socket for proxy
  proxy_socket_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (proxy_socket_fd_ == -1) {
    std::cerr << "Can not create soket" << std::endl;
    return false;
  }

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

  return true;
}

void TcpProxy::run() {
  for (;;) {
    int connect_fd = accept(proxy_socket_fd_, NULL, NULL);
    if (connect_fd == -1) {
      std::cerr << "Accept failed" << std::endl;
      close(proxy_socket_fd_);
      return;
    }
    std::cout << connect_fd << std::endl;
    /* perform read write operations ...
      read(ConnectFD, buff, size)
      */
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