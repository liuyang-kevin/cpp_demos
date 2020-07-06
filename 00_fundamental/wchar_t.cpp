/**
 *
 * 没有测试出来效果
 */
#include <iostream>
#include <locale>  //setlocale函数在locale头文件中定义
using namespace std;
int main() {
  //使用setlocale函数将本机的语言设置为中文简体
  // LC_ALL表示设置所有的选项（包括金融货币、小数点，时间日期格式、语言字符串的使用习惯等），
  // chs表示中文简体
  setlocale(LC_ALL, "chs");
  wchar_t wt[] =
      L"中国伟大复兴梦";  //大写字母L告诉编译器为"中"字分配两个字节的空间
  wcout << wt << endl;  //使用wcout来代替cout输出宽字符
  cout << "--------------" << endl;
  return 0;
}

/**
 * Mac系统，编译错误
 */
// #include <iostream>
// using namespace std;
// locale loc("chs");
// int main() {
//   wchar_t wStr[] = L"我使用了wcha_t类型";
//   wcout.imbue(loc);
//   wcout << wStr << endl;
//   return 0;
// }