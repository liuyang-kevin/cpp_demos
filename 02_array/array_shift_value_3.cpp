#include <stdlib.h>

#include <iostream>
using namespace std;
void Output(int *pBuffer, int nCount) {
  if (!pBuffer || !nCount) return;

  for (size_t i = 0; i < nCount; i++) {
    printf(" %d ", pBuffer[i]);
  }

  printf("\n");
}

/// 1. 取余数，计算有效移动
/// 2. 循环移动
/// 3. 三次反转算法，O(N)复杂度算法。 + 异或交换换值
void ShiftN(int *pBuffer, int nCount, int nShiftN) {
  if (!pBuffer || !nCount || !nShiftN) return;

  nShiftN %= nCount;

  int nIndex = 0;
  int nStart = nIndex;

  int nTemp = pBuffer[nIndex];

  for (size_t i = 0; i < nCount; i++) {
    nIndex = (nIndex + nShiftN) % nCount;
    // 异或交换 换值
    pBuffer[nIndex] ^= nTemp ^= pBuffer[nIndex] ^= nTemp;

    if (nIndex == nStart) {
      nStart++;
      nIndex = nStart;
      nTemp = pBuffer[nIndex];
    }
  }
}

int main(int argc, char *argv[]) {
  int buffer[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

  int nCount = sizeof(buffer) / sizeof(int);

  Output(buffer, nCount);

  ShiftN(buffer, nCount, 8);

  Output(buffer, nCount);

  return 0;
}