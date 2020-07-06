// https://www.runoob.com/cplusplus/cpp-functions.html

// Mac 下C++ ，请使用 -std=c++11 标准c++配置
// 例： g++ 04_function.cpp -o fun -std=c++11
#include <iostream>
using namespace std;

// 函数声明
int max(int num1, int num2);
int main() {
  // 局部变量声明
  int a = 100;
  int b = 200;
  int ret;

  // 调用函数来获取最大值
  ret = max(a, b);

  cout << "Max value is : " << ret << endl;

  auto tmpAddFunc = [](int x, int y) { return x < y; };

  bool result = tmpAddFunc(a, b);
  cout << "a" << (result ? "<" : ">") << "b" << endl;

  return 0;
}

// 函数返回两个数中较大的那个数
int max(int num1, int num2) {
  // 局部变量声明
  int result;

  if (num1 > num2)
    result = num1;
  else
    result = num2;

  return result;
}