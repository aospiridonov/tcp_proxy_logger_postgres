#pragma once

#include <cstring>

class Proxy {
public:
  virtual ~Proxy() = default;
  virtual bool init() = 0;
  virtual void run() = 0;
};
