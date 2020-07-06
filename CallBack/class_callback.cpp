#include <stdio.h>

#include <algorithm>
#include <iostream>

class Slave {
 public:
  typedef std::function<void(std::string)> CBPrint;

  void loop() {
    for (size_t i = 0; i < 20; i++) {
      /* code */
      p("aaaa");
    }
  }
  void setCallBack(CBPrint callback) { p = callback; }

 private:
  CBPrint p;
};

class Master {
 public:
  Slave slave;
  void runMaster() {
    // 这里必须是静态函数，不能是成员函数。
    slave.setCallBack(std::bind(&Master::mPrint, this, std::placeholders::_1));
    slave.loop();
    // std::cin.get();
  };
  void mPrint(std::string s) { std::cout << s << std::endl; }
};

int main(void) {
  // 实现class成员函数的callback
  Master m;
  m.runMaster();
  return 0;
}