LOCAL_PATH := $(call my-dir)/../../opusfile-0.12

include $(CLEAR_VARS)

# 编译所有平台 armeabi , armeabi-v7a , arm64-v8a , x86 , x86_64 , mips , mips64
APP_ABI := all
# 使用C++11标准编译
APP_CPPFLAGS +=-std=c++11
APP_STL := gnustl_shared
# 指定API等级
APP_PLATFORM :=android-19

# 输出的文件名称 libopusfile.so
LOCAL_MODULE        := opusfile

# 指定源码文件
LOCAL_SRC_FILES     := \
    $(LOCAL_PATH)/src/http.c \
    $(LOCAL_PATH)/src/info.c \
    $(LOCAL_PATH)/src/internal.c \
    $(LOCAL_PATH)/src/internal.h \
    $(LOCAL_PATH)/src/opusfile.c \
    $(LOCAL_PATH)/src/stream.c \
    $(LOCAL_PATH)/src/wincerts.c \
    $(LOCAL_PATH)/src/winerrno.h \

LOCAL_LDLIBS        := -lm -logg -lopus
LOCAL_C_INCLUDES    := \
    $(LOCAL_PATH)/include \

# 编译参数配置
LOCAL_CFLAGS        := -DNULL=0 -DSOCKLEN_T=socklen_t -DLOCALE_NOT_USED -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
LOCAL_CFLAGS        += -Drestrict='' -D__EMX__ -DOPUS_BUILD -DFIXED_POINT -DUSE_ALLOCA -DHAVE_LRINT -DHAVE_LRINTF -O3 -fno-math-errno
LOCAL_CPPFLAGS      := -DBSD=1 
LOCAL_CPPFLAGS      += -ffast-math -O3 -funroll-loops

include $(BUILD_SHARED_LIBRARY)