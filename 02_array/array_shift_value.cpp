/**
 * 数组左右移动，一个单位。移动步长需要循环。
 *
 * C语言memmove()函数：复制内存内容（可以处理重叠的内存块）。
 * memmove() 用来复制内存内容，其原型为：
 *   void * memmove(void *dest, const void *src, size_t num);
 *
 * memmove() 与 memcpy() 类似都是用来复制 src 所指的内存内容前 num 个字节到 dest
 * 所指的地址上。 不同的是，memmove() 更为灵活，当src 和 dest
 * 所指的内存区域重叠时，memmove() 仍然可以正确的处理，不过执行效率上会比使用
 * memcpy() 略慢些。
 *
 *
 * 个人感觉，没有 array_shift_value_2.cpp 中的3次反向序列效率高。
 */
#include <cstring>
#include <iostream>
using namespace std;
// rotate Left
void r_left(int *a, int n) {
  int tmp = a[0];
  memmove(a, a + 1, sizeof(int) * (n - 1));
  a[n - 1] = tmp;
}
// rotate right
void r_right(int *a, int n) {
  int tmp = a[n - 1];
  memmove(a + 1, a, sizeof(int) * (n - 1));
  a[0] = tmp;
}
void show(int *a, int n) {
  while (n--) cout << *a++ << ' ';
  cout << endl;
}

int main() {
  int ar[] = {1, 2, 3, 4, 5};
  int n = sizeof(ar) / sizeof(ar[0]);
  cout << n << endl;
  r_left(ar, n);
  show(ar, n);
  r_left(ar, n);
  show(ar, n);
  r_right(ar, n);
  show(ar, n);
  return 0;
}