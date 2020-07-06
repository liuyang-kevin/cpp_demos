#include <functional>  // std::function
#include <iostream>    // std::cout

class A {
 public:
  int i_ =
      0;  // C++11允许非静态（non-static）数据成员在其声明处（在其所属类内部）进行初始化

  void output(int x, int y) { std::cout << x << "" << y << std::endl; }
};

int main() {
  A a;
  // 绑定成员函数，保存为仿函数
  std::function<void(int, int)> fr =
      std::bind(&A::output, &a, std::placeholders::_1, std::placeholders::_2);
  // 调用成员函数
  fr(1, 2);

  // 绑定成员变量
  std::function<int&(void)> fr2 = std::bind(&A::i_, &a);
  fr2() = 100;  // 对成员变量进行赋值
  std::cout << a.i_ << std::endl;

  return 0;
}