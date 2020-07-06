/**
 * 无限循环
 * 如果条件永远不为假，则循环将变成无限循环。for
 * 循环在传统意义上可用于实现无限循环。
 *
 * 如果我们是服务类的程序，它便不能结束，那么无限循环，就是需要的。比如：
 *
 * 图形界面的窗体。
 * 持续性的数据库。
 * web服务，web网站。
 *
 * 其实，loop循环的程序，才是计算机的常态
 *
 * 执行本程序时，需要注意。死循环无法打断；开启一段事件后，你会发现电脑CPU便会疯狂运转。
 * 尤其C++输出的是二进制可执行程序，运行效率非常高，所以cpu的执行效率靠近100%
 *
 * win系统下，可以用任务管理器，找到程序进程，杀死进程；
 * linux/unix类系统下，
 * top                   #查看进程使用情况
 * ps aux | grep 03_loop #
 * ps是进程查询程序，grep是搜索程序，所有进程中搜索包含loop这个名字的进程，这里我们可以查看到进程号
 * kill -9 进程号         # kill是杀进程用的，杀掉那个进程
 */
#include <iostream>
using namespace std;
// step 1
// int main() {
//   for (;;) {
//     printf("This loop will run forever.\n");
//   }

//   return 0;
// }

// step 2
// 这样样的代码，执行起来风险太高了。
// 所以，我们的程序应该再有个中断操作。
//
// 这里会发现，循环不起效了。因为程序是顺序执行，等待键盘输入，这就是一个中断操作。
//
// 概念： 中断阻塞了程序，这块程序就是阻塞的，主进程，主线程被阻塞了。
// int main() {
//   int x;
//   for (;;) {
//     printf("This loop will run forever.\n");
//     cin >> x;
//     cout << "输入的 int 类型数字是 is x= " << x << endl;
//     if (x == 0) {
//       exit(0);  // exit这个函数，跟return效果是一样的，只不过它是一个退出函数
//     }
//   }

//   return 0;
// }

// step 3
// 一个简单的非阻塞，loop循环
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

static struct termios ori_attr, cur_attr;

int tty_reset(void) {
  if (tcsetattr(STDIN_FILENO, TCSANOW, &ori_attr) != 0) return -1;

  return 0;
}

int tty_set(void) {
  if (tcgetattr(STDIN_FILENO, &ori_attr)) return -1;
  memcpy(&cur_attr, &ori_attr, sizeof(cur_attr));
  cur_attr.c_lflag &= ~ICANON;
  // cur_attr.c_lflag |= ECHO;
  cur_attr.c_lflag &= ~ECHO;
  cur_attr.c_cc[VMIN] = 1;
  cur_attr.c_cc[VTIME] = 0;

  if (tcsetattr(STDIN_FILENO, TCSANOW, &cur_attr) != 0) return -1;

  return 0;
}

// 键盘输入
int kbhit(void) {
  fd_set rfds;
  struct timeval tv;
  int retval;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  /* Wait up to five seconds. */
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  // 对于select函数的功能简单的说就是对文件fd做一个测试。测试结果有三种可能：
  // 1.timeout=NULL（阻塞：select将一直被阻塞，直到某个文件描述符上发生了事件）
  // 2.timeout所指向的结构设为非零时间（等待固定时间：如果在指定的时间段里有事件发生或者时间耗尽，函数均返回）
  // 3.timeout所指向的结构，时间设为0（非阻塞：仅检测描述符集合的状态，然后立即返回，并不等待外部事件的发生）
  retval = select(1, &rfds, NULL, NULL, &tv);
  /* Don't rely on the value of tv now! */

  if (retval == -1) {
    perror("select()");
    return 0;

  } else if (retval) {
    return 1;
  }
  /* FD_ISSET(0, &rfds) will be true. */
  else {
    return 0;
  }

  return 0;
}

// 主体循环
void* subThread(void* unused) {
  int s_count = 100;

  int tty_set_flag;
  tty_set_flag = tty_set();

  // 循环
  while (s_count--) {
    printf(" loop....\n");

    usleep(80000);  // 微秒（百万分之一秒）。

    if (kbhit()) {  // 键盘有输入
      const int key = getchar();
      printf("%c", key);
      if (key == 'q') {
        if (tty_set_flag == 0) {
          tty_reset();
        }
        pthread_exit(0);
      }
    }
  }

  if (tty_set_flag == 0) {
    tty_reset();
  }
}

int main() {
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, &subThread, NULL);
  pthread_join(thread_id, NULL);
  return 0;
}