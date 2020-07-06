PROJECT(simple_player)

# 替换本机安装的include + 动态库地址
include_directories(/usr/local/include)
link_directories("/usr/local/Cellar/ffmpeg/4.2.1_2/lib")
link_directories("/usr/local/Cellar/sdl2/2.0.10/lib")
link_directories("/usr/local/Cellar/sdl2_image/2.0.5/lib")

set(FF_LIBS avdevice avfilter avformat avcodec swresample swscale avutil sdl2 sdl2_image)

SET(SRC_SOURCE main.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_SOURCE})

target_link_libraries(${PROJECT_NAME} ${FF_LIBS})
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14 -Wall -Werror)