// reading binary file
#include <iostream>
#include <fstream>

using namespace std;
const char * filename = "out.txt";

int main () {
    char * buffer;
    long size;
    ifstream in(filename, ios::in|ios::binary|ios::ate);
    size = in.tellg();
    in.seekg (0, ios::beg);
    buffer = new char[size];
    in.read (buffer, size);
    in.close();
    
    cout << "the complete file is in a buffer \n";
    cout << "buffer size " << size;
    
    delete[] buffer;
    return 0;
}