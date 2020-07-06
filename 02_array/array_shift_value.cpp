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
  r_left(ar, n);
  show(ar, n);
  r_right(ar, n);
  show(ar, n);
  return 0;
}