#include <iostream>

#include "src/tcp_proxy.h"

int main() {
  std::cout << "Run proxy" << std::endl;
  TcpProxy proxy;
  bool state = proxy.connect();
  if (state) {
    std::cout << "Connected" << std::endl;
  }
  proxy.run();

  // run_proxy(proxy);
  std::cout << "Stop proxy" << std::endl;
  return 0;
}