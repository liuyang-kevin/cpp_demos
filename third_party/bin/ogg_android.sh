#!bash
mkdir ./output
rm -rf ./output/ogg-android/*
mkdir ./output/ogg-android
mkdir ./output/ogg-android/libs

#手动设置开始
CMAKE_DIR=$ANDROID_HOME/cmake/3.6.4111459
NDK_DIR=$ANDROID_HOME/ndk-bundle
SDK_DIR=$ANDROID_HOME
#手动设置结束
#cmake_exec=$CMAKE_DIR/bin/cmake
cmake_exec=cmake
#ninja_exec=$CMAKE_DIR/bin/ninja
ninja_exec=ninja
cur_dir=$(pwd)

# 中间文件 输出文件夹
out_dir=$cur_dir/output/ogg-android/build
# 二进制输出文件夹
libout_dir=$cur_dir/output/ogg-android/libs
# 项目
H_dir=$cur_dir/libogg-1.3.4
# cmake 缓存
B_dir=$cur_dir/output/ogg-android/build


CMAKE_BUILD_TYPE=Debug
CMAKE_MAKE_PROGRAM=$ninja_exec
CMAKE_TOOLCHAIN_FILE=$NDK_DIR/build/cmake/android.toolchain.cmake
ANDROID_NATIVE_API_LEVEL=23
ANDROID_TOOLCHAIN=clang
CMAKE_GENERATOR="Ninja"
BUILD_SHARED_LIBS=TRUE



APP_ABI=(armeabi-v7a arm64-v8a x86 x86_64)

for t in ${APP_ABI[*]}; do
echo "building $t"
# TARGET=armeabi-v7a
ANDROID_ABI=$t
ANDROID_NDK=
CMAKE_LIBRARY_OUTPUT_DIRECTORY=$libout_dir/$t/

$cmake_exec -H$H_dir -B$B_dir \
-DANDROID_ABI=$ANDROID_ABI \
-DANDROID_NDK=$ANDROID_NDK \
-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$CMAKE_LIBRARY_OUTPUT_DIRECTORY  \
-DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
-DCMAKE_MAKE_PROGRAM=$CMAKE_MAKE_PROGRAM  \
-DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE \
-DANDROID_NATIVE_API_LEVEL=$ANDROID_NATIVE_API_LEVEL \
-DANDROID_TOOLCHAIN=$ANDROID_TOOLCHAIN \
-DCMAKE_GENERATOR=$CMAKE_GENERATOR \
-DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS

cd $out_dir
$ninja_exec 
cd $cur_dir
rm -rf $out_dir
done





# # cmake -version #检验是否安装成功，显示对应 CMake 版本号即表示安装成功
# # cd ./output/ogg-android && cmake ../../build_config_files/ogg
# cd ./output/ogg-android && cmake \
# -H../../libogg-1.3.4 \
# -Bpath/to/generated/ninja/project/debug/ABI \
# -DANDROID_ABI=all \
# -DANDROID_PLATFORM=android-19 \
# -DANDROID_NDK=android-sdk/ndk/ndk-version \
# -DCMAKE_TOOLCHAIN_FILE=android-sdk/ndk/ndk-version/build/cmake/android.toolchain.cmake \
# -G Ninja