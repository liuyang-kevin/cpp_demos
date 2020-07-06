/*非阻塞输入
 *普通的scanf,getchar等函数必须在敲回车之后方可输入方能结束进入运算，非阻塞输入即每输入一个字符就参与运算
 *搜索框的模糊查询即可以采用非阻塞输入
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL | ICANON)

class Input {
 public:
  // Input构造函数即非阻塞输入
  Input() {
    tcgetattr(STDIN_FILENO, &old_);
    new_ = old_;
    new_.c_lflag &= ~ECHOFLAGS;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_);

    int flag = fcntl(STDIN_FILENO, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flag);
  }
  //析构函数，这里将光标定位
  ~Input() {
    tcsetattr(0, 0, &old_);
    printf("\033[%dH", h + 2);
  }
  //接受字符，这里接受到的所有字符不再有特殊含义，比如接收到DEL键，只会返回SCII值
  //127
  int get() {
    c = getchar();
    if (c == '\033')  // ESC
    {
      c = getchar();

      if (c == '[') {
        c = getchar();
        switch (c) {
          case 'A':  //上 键，该键是一个组合键
            return 1;
          case 'B':  //下
            return 2;
          case 'C':  //左
            return 3;
          case 'D':  //右
            return 4;
        }
      }
      return 0;
    }
    return c;
  }
  int h;  //用于标示光标的位置
 private:
  struct termios old_;
  struct termios new_;
  char c;
  static int j;
};

int main() {
  Input t;
  int i;
  t.h = 1;
  int count = 0;
  int j = 0;
  char s[1024] = {
      '\0',
  };
  while (1) {
    i = t.get();
    if (i == 0) {  //如果i是ESC
      break;
    }
    if (i > 0) {
      if (i == '\n' && count != 0) {  //如果i是回车
        printf("\n");
        break;
      } else if (i == '\b' ||
                 i == 127 && count > 0) {  //如果i是DEL或是BackSpace
        s[--j] = '\0';
        count--;
      } else if (i == 1) {
        printf("UP");
      } else if (i == 2) {
        printf("DOWN");
      } else if (i == 3) {
        printf("LEFT");
      } else if (i == 4) {
        printf("RIGHT");
      } else {
        s[j++] = i;
        count++;
      }
      system("clear");  //清屏
      printf("\033[%dH%s <", t.h + 1, s);
      printf("\033[%dH\033[33m%s\033[0m", t.h, s);
    }
  }
}