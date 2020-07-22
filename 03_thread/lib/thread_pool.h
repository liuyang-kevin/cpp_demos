/**
 * C++11
 * “管理一个任务队列，一个线程队列，然后每次取一个任务分配给一个线程去做，循环往复。”
 *
 * std::function
 *  是一个函数包装器模板，
 *  auto g_Minus = [](int i, int j){ return i - j; };
 *  function<int(int, int)> f = g_Minus;
 *
 * std::vector
 *  向量是表示可以动态改变大小的数组的序列容器。特性：有序、动态数组、自动存储
 *
 * std::queue
 *  队列
 *
 * std::mutex
 *  进程互斥（mutual exclusion）
 *  Mutex 又称互斥量，C++ 11中与 Mutex 相关的类（包括锁类型）和
 *  函数都声明在 <mutex> 头文件中
 *
 * std::condition_variable
 *  当 std::condition_variable 对象的某个 wait 函数被调用的时候，它使用
 *  std::unique_lock(通过std::mutex)来锁住当前线程。当前线程会一直被阻塞，
 *  直到另外一个线程在相同的std::condition_variable 对象上调用了 notification
 *  函数来唤醒当前线程。
 *
 * std::atomic
 *  C++11 中最简单的原子类型 std::atomic_flag，但是 std::atomic_flag 过于简单，
 *  只提供了 test_and_set 和 clear 两个 API，不能满足其他需求.
 *  std::atomic提供了如 store, load, exchange, compare_exchange 等
 *
 * 1. 一个线程 pool，一个任务队列 queue ，应该没有意见；
 * 2. 任务队列是典型的生产者-消费者模型，本模型至少需要两个工具：
 *    一个 mutex + 一个条件变量，或是一个 mutex + 一个信号量。
 *  1. mutex 实际上就是锁，保证任务的添加和移除(获取)的互斥性，
 *  2. 一个条件变量是保证获取 task 的同步性：一个 empty
 * 的队列，线程应该等待(阻塞)；
 * 3. atomic<bool> 本身是原子类型，从名字上就懂：
 *    它们的操作 load()/store() 是原子操作，所以不需要再加 mutex。
 */
#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace std {
#define MAX_THREAD_NUM 256

//线程池,可以提交变参函数或拉姆达表达式的匿名函数执行,可以获取执行返回值
//不支持类成员函数, 支持类静态成员函数或全局函数,Opteron()函数等
class threadpool {
  using Task = std::function<void()>;
  // 线程池
  std::vector<std::thread> pool;
  // 任务队列
  std::queue<Task> tasks;
  // 同步
  std::mutex m_lock;
  // 条件阻塞
  std::condition_variable cv_task;
  // 是否关闭提交
  std::atomic<bool> stoped;
  //空闲线程数量
  std::atomic<int> idlThrNum;

 public:
  inline threadpool(unsigned short size = 4) : stoped{false} {
    idlThrNum = size < 1 ? 1 : size;
    for (size = 0; size < idlThrNum; ++size) {  //初始化线程数量
      pool.emplace_back([this] {                // 工作线程函数
        while (!this->stoped) {
          std::function<void()> task;
          {  // 获取一个待执行的 task
            std::unique_lock<std::mutex> lock{
                this->m_lock};  // unique_lock 相比 lock_guard
                                // 的好处是：可以随时 unlock() 和 lock()
            this->cv_task.wait(lock, [this] {
              return this->stoped.load() || !this->tasks.empty();
            });  // wait 直到有 task
            if (this->stoped && this->tasks.empty()) return;
            task = std::move(this->tasks.front());  // 取一个 task
            this->tasks.pop();
          }
          idlThrNum--;
          task();
          idlThrNum++;
        }
      });
    }
  }
  inline ~threadpool() {
    stoped.store(true);
    cv_task.notify_all();  // 唤醒所有线程执行
    for (std::thread& thread : pool) {
      // thread.detach(); // 让线程“自生自灭”
      if (thread.joinable())
        thread.join();  // 等待任务结束， 前提：线程一定会执行完
    }
  }

 public:
  // 提交一个任务
  // 调用.get()获取返回值会等待任务执行完,获取返回值
  // 有两种方法可以实现调用类成员，
  // 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
  // 一种是用 mem_fn： .commit(std::mem_fn(&Dog::sayHello), &dog)
  template <class F, class... Args>
  auto commit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    if (stoped.load())  // stop == true ??
      throw std::runtime_error("commit on ThreadPool is stopped.");

    using RetType =
        decltype(f(args...));  // typename std::result_of<F(Args...)>::type,
                               // 函数 f 的返回值类型
    auto task = std::make_shared<std::packaged_task<RetType()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));  // wtf !
    std::future<RetType> future = task->get_future();
    {  // 添加任务到队列
      std::lock_guard<std::mutex> lock{
          m_lock};  //对当前块的语句加锁  lock_guard 是 mutex 的 stack
                    //封装类，构造的时候 lock()，析构的时候 unlock()
      tasks.emplace([task]() {  // push(Task{...})
        (*task)();
      });
    }
    cv_task.notify_one();  // 唤醒一个线程执行

    return future;
  }

  //空闲线程数量
  int idlCount() { return idlThrNum; }
};

}  // namespace std
#endif