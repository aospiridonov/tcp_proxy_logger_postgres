#pragma once

class Proxy {
public:
  virtual ~Proxy() = default;
  virtual bool connect() = 0;
  virtual void run(){};
};
