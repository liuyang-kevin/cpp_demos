#include <fstream>

// ofstream         //文件写操作 内存写入存储设备 
// ifstream         //文件读操作，存储设备读区到内存中
// fstream          //读写操作，对打开的文件可进行读写操作
using namespace std;
int main () {
    // 可能会碰到ofstream out("Hello.txt"), ifstream in("..."),fstream foi("...")这样的的使用，并没有显式的去调用open（）
    ofstream out("out.txt");
    if (out.is_open()) {
            out << "This is a line.\n";
            out << "This is another line.\n";
            out.close();
    }
    return 0;
}