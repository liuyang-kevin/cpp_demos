#include <iostream>
#include <memory>
#include <utility>
// g++ unique_ptr_1.cpp -o unique_ptr1 -std=c++14 && ./unique_ptr1
class Foo {
 public:
  Foo() = default;
  Foo(int a) : _a(a) {}
  ~Foo() {}
  int get_a() { return _a; }
  void set_a(int a) { _a = a; }

 private:
  int _a;
};

std::unique_ptr<Foo> change_a(std::unique_ptr<Foo> f) {
  f->set_a(10);
  return f;  //返回值是右值操作，所以不需要std::move
}

int main() {
  // std::make_unique是c++14才有
  std::unique_ptr<Foo> pf = std::make_unique<Foo>(10);
  // unique_ptr的复制构造函数和拷贝构造函数是删除了的，这样保证了对象独占，如果不是独占，那么跟shared_ptr
  // 就是一样的功能了。
  // std::unique_ptr<Foo> pf1 = pf;  // compile error

  // 按值传参，会有拷贝问题，同上
  // auto p = change_a(pf);   //compile error

  auto p = change_a(std::move(pf));
  std::cout << "get_a = " << p->get_a() << std::endl;
  if (!pf) {
    std::cout << "pf is nullptr" << std::endl;
  }
  // owner transfer from function
  std::unique_ptr<Foo> pf2 = std::make_unique<Foo>(11);
  std::unique_ptr<Foo> p2 = change_a(std::move(pf2));
  std::cout << "get_a = " << p2->get_a() << std::endl;
  if (!pf2) {
    std::cout << "pf2 is nullptr" << std::endl;
  }

  //使用reset
  pf2.reset(new Foo(12));
  std::cout << "pf2 is not null: " << pf2->get_a() << std::endl;

  // release获取原始指针
  Foo* ff = pf2.release();
  if (!pf2) {
    std::cout << "pf2 is nullptr" << std::endl;
  }
  std::cout << "ff is not null: " << ff->get_a() << std::endl;

  return 0;
}