# 各种动态库编译的文档记录
* bin 目录脚本
* build_config_files 一些脚本
* 项目放到项目当前路径

# OPUS
opus是xiph设计的一种PCM的压缩算法，如果要保存直接播放的媒体文件，使用ogg封包。

https://gitlab.xiph.org/xiph/libopusenc

1. 官方的库opus只是提供了PCM的压缩算法
2. opusenc、opusfile、opus-tools，都是opus的扩展、工具库。
    * 写代码，直接调用功能不是很全，不适合使用，小项目不如自己写特定方法
    * 复杂的，推荐ffmpeg