# 简介
这是一个c++练习用的文档。内容比较杂。但都是一个文件一个功能的代码实现。

绝大多数单程序不需要Makefile跟CMake去配置项目，

可以用最简单原始的的角度去查看C++代码生成二进制程序。

# 工具配置
* 安装格式化代码工具
    * brew install clang-format 
* 配置settings
    * clang-format配置，写绝对路径
    ```text
    "C_Cpp.clang_format_fallbackStyle": "Google",
    "C_Cpp.clang_format_path": "/usr/local/bin/clang-format",
    "editor.formatOnSave": true,
    "files.autoSave": "onFocusChange"

    Mac下多一个编译标准采用-std=c++11的配置，Mac的gcc有点特殊。
    "code-runner.executorMap": {
        "cpp": "cd $dir && g++ $fileName -o $fileNameWithoutExt -std=c++11 && $dir$fileNameWithoutExt",
    ```
# 代码索引
没有明确写明的文件，都是临时文件，没有删除。
* 基础知识，着眼于c++的特殊性
    * ./00_fundamental
* 类 与 标准库
    * ./01_class_and_more 只准备了文件夹，概念都理解。
    * https://www.runoob.com/cplusplus/cpp-classes-objects.html
* 文件操作
    * ./fstream
* 媒体文件解码
    * [ffmpeg demos（其他学习项目）](https://github.com/liuyang-kevin/ffmpeg-tutorial)
    * ./ffmpeg
        * 安装ffmpeg库，brew install ffmpeg
        * 手动配置路径，需修改代码引用
        * ffmpeg文件夹内，部分代码、使用了clion+cmake配置
* [小游戏合集 （其他学习项目）](https://github.com/liuyang-kevin/cpp_sample_games)