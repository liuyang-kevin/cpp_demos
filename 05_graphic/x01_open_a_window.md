http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/#building-on-mac

部分翻译+二次整理
# 安装
* OpenGL的安装。安装对库就行了
* Cmake管理项目，根据自己需要安装
* C/C++、gcc等等IDE或编译器
    * Mac上XCode是必须的，躲不开，其他的brew都能装
    * Linux根据需要
    * win上一般是VS20xx
* 窗口库，这个比较庞杂 / Windowing & misc
    * [GLFW](https://www.glfw.org/)，GLFW用C编写，并支持Windows，macOS，X Window System和Wayland协议。
    * GLUT, 最老的。没有维护，可怕的许可证。不建议。
    * [FreeGlut](http://freeglut.sourceforge.net/)，相同的API，但是开源。
    * [SDL](https://www.libsdl.org/) 这个也不错，有些人喜欢
    * [SFML](https://www.sfml-dev.org/index-fr.php). 这是个富库，音频，硬件接口，多语言API兼容
# GLFW
我们将使用GLFW，
```shell
# 我是Mac测试的这个demo，写代码的时候，发现GL/glew.h我以前没装
brew install glew
brew install glfw3
```
## glew
OpenGL扩展库是个简单的工具，用于帮助C/C++开发者初始化扩展（OpenGL扩展功能）并书写可移植的应用程序。GLEW当前支持各种各样的操作系统，包含Windows，Linux，Darwin，Irix与Solaris。
### win glew 下载 
http://glew.sourceforge.net/

# 手动编译，懒得用CMake
```shell
# 查看本机gcc版本
gcc --version
cd ../05_graphic
# 直接编译，这里会发现错误，因为我们还没链接glfw，glew等库
gcc ./x01_open_a_window.cpp -o openWin.bin
# 手动链接3个库， framework是mac特有，win，linux自己寻找OpenGL如何链接
gcc ./x01_open_a_window.cpp -o openWin.bin -lglew -lglfw -framework OpenGL
# 追加自动运行
gcc ./x01_open_a_window.cpp -o openWin.bin -lglew -lglfw -framework OpenGL && ./openWin.bin
```