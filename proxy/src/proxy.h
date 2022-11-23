#pragma once

#include <string>

class Proxy {
public:
  virtual ~Proxy() = default;
  virtual bool init() = 0;
  virtual void run() = 0;
  virtual void log(const std::string &data) = 0;
};
