// obtaining file size
#include <iostream>
#include <fstream>
using namespace std;
const char * filename = "out.txt";

int main () {
    long l,m;
    ifstream in(filename, ios::in|ios::binary);
    l = in.tellg();
    in.seekg (0, ios::end);
    m = in.tellg();
    in.close();
    cout << "size of " << filename;
    cout << " is " << (m-l) << " bytes.\n";


    // 直接打开文件尾部，计算文件长度
    ifstream in1(filename, ios::ate|ios::binary);
    cout << "size of " << filename;
    cout << " is " << in1.tellg() << " bytes.\n";
    in1.close();

    return 0;
}
