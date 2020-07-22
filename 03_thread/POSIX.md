https://www.ibm.com/developerworks/cn/linux/thread/posix_thread1/index.html

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