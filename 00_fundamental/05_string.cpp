#include <cstring>
#include <iostream>

using namespace std;
int main() {
  /**
   * 首先是字符串的两种风格，
   *
   * C 风格字符串
   * C++ 引入的 string 类类型
   *
   * 因为C++是c的扩展，C里一大繁琐的东西就是字符串操作。
   * 其实C本没有字符串这个概念，只有char，
   * 所谓char，其实就是一快内存上的一个字符对应的值。
   * 把它连续起来，就是所谓的“字符串”了，通常一个字符串，需要用“\0”来结尾。
   * 这个是cpu需要的，用来标记字符串结束了
   *
   * c的char类型，其实就是数组，数组其实就是指针。只是写法上让程序猿觉得更好理解。
   *
   */
  char greeting[6] = {'H', 'e', 'l', 'l', 'o', '\0'};

  cout << "greeting[6] 写法: ";
  cout << greeting << endl;

  cout << "----------------------------------" << endl;
  char g2[] = "hello";
  cout << "g2[] 数组写法" << g2 << endl;

  cout << "----------------------------------" << endl;
  //   char *g3 = "hello";
  auto g3 =
      "hello";  // 这里是一个c++规范，char *这种写法不标准了，auto就是代替的它
  cout << "char* 指针写法" << g3 << endl;
  cout << "----------------------------------" << endl;

  /***
   * 此处呢，就是理解一下，c操作字符串怎么费劲。
   * **/
  char str1[11] = "Hello";
  char str2[11] = "World";
  char str3[11];
  int len;

  // 复制 str1 到 str3
  strcpy(str3, str1);
  cout << "strcpy( str3, str1) : " << str3 << endl;

  // 连接 str1 和 str2
  strcat(str1, str2);
  cout << "strcat( str1, str2): " << str1 << endl;

  // 连接后，str1 的总长度
  len = strlen(str1);
  cout << "strlen(str1) : " << len << endl;
  cout << "----------------------------------" << endl;
  cout << "C++的String类" << endl;
  cout << "从代码角度，就可以看出，string类将c繁琐的字符操作封装到String类型当"
          "中去了"
       << endl;
  string str_1 = "Hello";
  string str_2 = "World";
  string str_3;
  int string_len;

  // 复制 str1 到 str3
  str_3 = str_1;
  cout << "str_3 : " << str3 << endl;

  // 连接 str1 和 str2
  str_3 = str_1 + str_2;
  cout << "str_1 + str_2 : " << str_3 << endl;

  // 连接后，str3 的总长度
  string_len = str_3.size();
  cout << "str_3.size() :  " << string_len << endl;
  cout << "----------------------------------" << endl;
  return 0;
}