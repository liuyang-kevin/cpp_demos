#include <stdio.h>

#include <algorithm>
#include <iostream>

class Slave {
 public:
  void loop() {
    for (size_t i = 0; i < 20; i++) {
      /* code */
      p("aaaa");
    }
  }
  void setCallBack(void (*callback)(std::string s)) { p = callback; }

 private:
  void (*p)(std::string s);
};

class Master {
 public:
  Slave slave;
  void runMaster() {
    // 这里必须是静态函数，不能是成员函数。
    slave.setCallBack(mPrint);
    slave.loop();
    // std::cin.get();
  };
  static void mPrint(std::string s) { std::cout << s << std::endl; }
};

//回调函数
int ADD(int (*callback)(int, int), int a, int b) {
  return (*callback)(a, b);  //此处回调add函数...
}
//普通函数
int add(int a, int b) { return a + b; }

int main(void) {
  printf("%d\n", add(1, 2));
  printf("%d\n", ADD(add, 1, 2));

  // 这里展现的是class的静态函数作为callback
  Master m;
  m.runMaster();
  return 0;
}