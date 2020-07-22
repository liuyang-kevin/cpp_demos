# Mutex 进程互斥（mutual exclusion）。
mutex一般用于为一段代码加锁，以保证这段代码的原子性（atomic）操作，即：要么不执行这段代码，要么将这段代码全部执行完毕。

> 最简单的并发冲突问题：
>
>    一个变量自增1：`balance = balance + 1;`
```C
// 汇编中 指令集操作过程中会引入中间变量来保存右边的值,
int tmp = balance + 1;
balance = tmp;
// 这就需要一把互斥锁（mutual exclusive, mutex）
// 达到任何一个线程“要么全部执行上述代码，要么不执行这段代码”的效果
lock_t mutex;
...
lock(&mutex)
    balance = balance + 1;
unlock(&mutex);
```

## spin lock，弹簧锁 算法
```C
typedef struct __lock_t { int flag; } lock_t;

void init(lock_t *mutex) {
    // 0: lock is available
    // 1: lock is held
    mutex->flag = 0;
}

void lock(lock_t *mutex) {
    while (mutex->flag == 1) {  // Test the flag.
        ;    // Wait the lock
    mutex->flag = 1;  // Set the lock, i.e. start to hold lock
}

void unlock(lock_t *mutex) {
    mutex->flag = 0;
}
```
这里的lock函数，必须依靠指令集级别的支持才能真正做到atomic。

lock函数中的while操作，当A、B线程同时走到此，由于mutex->flag还未被赋值为1，B同样可以跳出while，从而跟A一样拿到这把锁！

关键问题就在于：
* 对mutex->flag的检测
* 对mutex->flag的赋值
这两个操作必须是不被干扰的，也就是它必须是atomic的，

要么这两段代码不被执行，要么这两段代码被不中断地完整执行。

这就需要借助CPU指令集的帮助，来保证上述两条语句的atomic操作，也即是著名的TestAndSet()操作。
```C
int TestAndSet(int *ptr, int new) {
    int old = *ptr;
    *ptr = new; 
    return old;
}
void lock(lock_t *mutex) {
    while (TestAndSet(&mutex->flag, 1) == 1); //这里修改为这个原子操作，进行判断
    // mutex->flag = 1; 这句也不再需要，TestAndSet完成了set动作
}
```

这样的支持TestAndSet()的实现，便是最简单的spin lock，弹簧锁。之所以叫弹簧锁，那是因为在各类锁当中，弹簧锁就是最初的被投入工业使用的最简单的实现技术。

## 硬同步。
之所以叫硬同步，是因为TestAndSet()函数中的三条语句是由硬件保证同步的，即硬件保证这三条语句必须原子运行，中间不发生任何中断，如同一条语句。