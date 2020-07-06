/***
 * 一种复杂度为：O(N)的方法，原理如下：
 * 假设原序列为：abcd1234
 * 移位4位后为：1234abcd
 *
 * 移位过程可以表示成如下步骤：
 * 逆序排列abcd：dcba1234
 * 逆序排列1234：dcba4321
 * 逆序排列dcba4321：1234abcd
 *
 * 也就是说需要编写一个逆序函数，并进行三次逆序变换。
 *
 *
 */

#include <stdlib.h>

#include <iostream>
using namespace std;
void Reverse(int *arr, int b, int e);
int s[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
int main() {
  int k, kk, N;
  N = 10;       // 数组长
  k = 99;       // 位移次数
  kk = k % 10;  // 取余数，有效移动
  //下面为循环移位部分
  Reverse(s, 0, N - kk - 1);  // 0到（移动点）反转
  Reverse(s, N - kk, N - 1);  // 移动点到末尾，反转
  Reverse(s, 0, N - 1);       // 整体反转。
  //显示
  for (int i = 0; i <= 9; i++) {
    cout << s[i];
  }
  cout << endl;
}
void Reverse(int *arr, int b, int e) {
  int temp;
  for (; b < e; b++, e--) {
    temp = arr[e];
    arr[e] = arr[b];
    arr[b] = temp;
  }
}