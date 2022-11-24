#include <iostream>

#include "src/tcp_proxy.h"

int main() {
  TcpProxy proxy;
  bool state = proxy.init();
  if (state) {
    std::cout << "Connected" << std::endl;
    std::cout << "Run proxy" << std::endl;
    proxy.run();
    std::cout << "Stop proxy" << std::endl;
  }
  return 0;
}