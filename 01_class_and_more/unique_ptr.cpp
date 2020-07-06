#include <cassert>  // assert
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>  // std::unique_ptr
#include <vector>

/*
 *  c++11 std::unique_ptr demo
 *  作用就是给对象包一个指针，再附加一个Deleter，删除器，释放器。
 *  好处就是，再定义，使用之前，就做好释放操作，这个包裹指针结束，里面的内容也就释放了。
 *  这就等于是go里的defer
 *
 *  1. unique_ptr独占管理对象，只有移动语义。
 *      * 包裹
 *  2.
 * unique_ptr可以不占用对象，即为空。可以通过reset()或者赋值nullptr释放管理对象。
 *      * 可以不包内容
 *  配合 std::move()操作，可以吧包裹的内容交换给别人。
 *
 */

// std::make_unique是c++14才有的特性,所以编译命令是-std=c++14
// g++ unique_ptr.cpp -o unique_ptr -std=c++14 && ./unique_ptr

struct B {
  virtual void bar() { std::cout << "B::bar\n"; }
  virtual ~B() = default;
};
struct D : B {
  D() { std::cout << "D::D\n"; }
  ~D() { std::cout << "D::~D\n"; }
  void bar() override { std::cout << "D::bar\n"; }
};

// 消费 unique_ptr 的函数能以值或以右值引用接收它
std::unique_ptr<D> pass_through(std::unique_ptr<D> p) {
  p->bar();
  return p;
}

void close_file(std::FILE* fp) { std::fclose(fp); }

int main() {
  std::cout << "unique ownership semantics demo\n";
  {
    auto p = std::make_unique<D>();  // p 是占有 D 的 unique_ptr
    auto q = pass_through(std::move(p));
    assert(!p);  // 现在 p 不占有任何内容并保有空指针
    q->bar();    // 而 q 占有 D 对象
  }              // ~D 调用于此

  std::cout << "Runtime polymorphism demo\n";
  {
    std::unique_ptr<B> p = std::make_unique<D>();  // p 是占有 D 的 unique_ptr
                                                   // 作为指向基类的指针
    p->bar();  // 多态

    std::vector<std::unique_ptr<B>> v;  // unique_ptr 能存储于容器
    v.push_back(std::make_unique<D>());
    v.push_back(std::move(p));
    v.emplace_back(new D);
    for (auto& p : v) p->bar();  // 多态
  }                              // ~D called 3 times

  std::cout << "Custom deleter demo\n";
  std::ofstream("demo.txt") << 'x';  // 准备要读的文件
  {
    std::unique_ptr<std::FILE, void (*)(std::FILE*)> fp(
        std::fopen("demo.txt", "r"), close_file);
    if (fp)  // fopen 可以打开失败；该情况下 fp 保有空指针
      std::cout << (char)std::fgetc(fp.get()) << '\n';
  }  // fclose() 调用于此，但仅若 FILE* 不是空指针
     // （即 fopen 成功）

  std::cout << "Custom lambda-expression deleter demo\n";
  {
    std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr) {
      std::cout << "destroying from a custom deleter...\n";
      delete ptr;
    });  // p 占有 D
    p->bar();
  }  // 调用上述 lambda 并销毁 D

  std::cout << "Array form of unique_ptr demo\n";
  { std::unique_ptr<D[]> p{new D[3]}; }  // 调用 ~D 3 次

  std::cout << "demo1\n";
  {
    std::unique_ptr<D[]> p{new D[3]};
    p.reset();
    assert(!p);
  }
  std::cout << "demo2\n";
  {
    std::unique_ptr<D[]> p{new D[3]};
    p = nullptr;
    assert(!p);
  }

  std::cin.get();
  // system("pause");  //没有这个，控制台程序会一闪即过，你来不及看到执行结果
  /**
   * 但只适合DOS或Windows，不合适合Linux，这就是出现“sh: PAUSE: command not
   * found" 的原因，在Linux可直接调用pause()。
   * 调用系统命令system()去实现暂停程序，耗费系统资源。
   * C中推荐使用getchar(); C++中使用cin.get();
   *
   * **/
}