#!bash
mkdir ./output
mkdir ./output/opus-android
ndk-build APP_BUILD_SCRIPT=build_config_files/opus/Android.mk NDK_PROJECT_PATH=./output/opus-android