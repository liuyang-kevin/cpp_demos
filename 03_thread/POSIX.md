# POSIX是标准，与程序本身无关
可移植操作系统接口（英语：Portable Operating System Interface，缩写为POSIX）是IEEE为要在各种UNIX操作系统上运行软件，而定义API的一系列互相关联的标准的总称，其正式称呼为IEEE Std 1003，而国际标准名称为ISO/IEC 9945。此标准源于一个大约开始于1985年的项目。POSIX这个名称是由理查德·斯托曼（RMS）应IEEE的要求而提议的一个易于记忆的名称。它基本上是Portable Operating System Interface（可移植操作系统接口）的缩写，而X则表明其对Unix API的传承。

----
# POSIX线程是线程，跟POSIX进程不是一个东西
POSIX线程（英语：POSIX Threads，常被缩写为Pthreads）是POSIX的线程标准，定义了创建和操纵线程的一套API。

> 不同的系统，实现POSIX的方式也不同

[Linux 线程实现机制分析](https://www.ibm.com/developerworks/cn/linux/kernel/l-thread/index.html)

----
[pthread](https://www.ibm.com/developerworks/cn/linux/thread/posix_thread1/index.html)

```c
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
 void *thread_function(void *arg) {
  int i;
  for ( i=0; i<20; i++) {
    printf("Thread says hi!\n");
    sleep(1);
  }
  return NULL;
}
int main(void) {
  pthread_t mythread;
   
  if ( pthread_create( &mythread, NULL, thread_function, NULL) ) {
    printf("error creating thread.");
    abort();
  }
  if ( pthread_join ( mythread, NULL ) ) {
    printf("error joining thread.");
    abort();
  }
  exit(0);
}
```

* main() 中声明了变量 mythread，类型是 pthread_t。
    * pthread_t 类型在 pthread.h 中定义，通常称为“线程 id”（缩写为 "tid"）。可以认为它是一种线程句柄。
    * 记住 mythread 只是一个 "tid"，
* 调用 pthread_create 函数创建一个真实活动的线程。
    * 第一个参数 &mythread 是指向 mythread 的指针。
    * 第二个参数当前为 NULL，可用来定义线程的某些属性。由于缺省的线程属性是适用的，只需将该参数设为 NULL。
    * 第三个参数是新线程启动时调用的函数名。本例中，函数名为 thread_function()。
    * 第四个参数。向线程传递一个任意参数,本例中，因为没有必要将任何数据传给微不足道的 thread_function()，所以将第四个参数设为 NULL。
*  pthread_join() 将两个线程合并为一个线程。
    * pthread_join() 的第一个参数是 tid mythread。
    * 第二个参数是指向 void 指针的指针。(子线程函数的返回值)

# 无父，无子
POSIX 线程中不存在这种层次关系。虽然主线程可以创建一个新线程，新线程可以创建另一个新线程，POSIX 线程标准将它们视为等同的层次。

```c
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
int myglobal;
 void *thread_function(void *arg) {
  int i,j;
  for ( i=0; i<20; i++) {
    j=myglobal;
    j=j+1;
    printf(".");
    fflush(stdout);
    sleep(1);
    myglobal=j;
  }
  return NULL;
}
int main(void) {
  pthread_t mythread;
  int i;
  if ( pthread_create( &mythread, NULL, thread_function, NULL) ) {
    printf("error creating thread.");
    abort();
  }
  for ( i=0; i<20; i++) {
    myglobal=myglobal+1;
    printf("o");
    fflush(stdout);
    sleep(1);
  }
  if ( pthread_join ( mythread, NULL ) ) {
    printf("error joining thread.");
    abort();
  }
  printf("\nmyglobal equals %d\n",myglobal);
  exit(0);
}
```
按照同步的思想程序结束时 myglobal 值应当等于 40。但是 myglobal 输出结果为 21，

这是因为每个线程内部执行时是并发的，局部变量j拿到的是20.并没有改变全局myglobal

-----