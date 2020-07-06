//在此处跑在子线程中，并回调到java层
void download(void *p) {
  if (p == NULL) return;

  JNIEnv *env;
  //获取当前native线程是否有没有被附加到jvm环境中
  int getEnvStat = (*g_VM)->GetEnv(g_VM, (void **)&env, JNI_VERSION_1_6);
  if (getEnvStat == JNI_EDETACHED) {
    //如果没有， 主动附加到jvm环境中，获取到env
    if ((*g_VM)->AttachCurrentThread(g_VM, &env, NULL) != 0) {
      return;
    }
    mNeedDetach = JNI_TRUE;
  }
  //强转回来
  jobject jcallback = (jobject)p;

  //通过强转后的jcallback 获取到要回调的类
  jclass javaClass = (*env)->GetObjectClass(env, jcallback);

  if (javaClass == 0) {
    LOG("Unable to find class");
    (*g_VM)->DetachCurrentThread(g_VM);
    return;
  }

  //获取要回调的方法ID
  jmethodID javaCallbackId =
      (*env)->GetMethodID(env, javaClass, "onProgressChange", "(JJ)I");
  if (javaCallbackId == NULL) {
    LOGD("Unable to find method:onProgressCallBack");
    return;
  }
  //执行回调
  (*env)->CallIntMethod(env, jcallback, javaCallbackId, 1, 1);

  //释放当前线程
  if (mNeedDetach) {
    (*g_VM)->DetachCurrentThread(g_VM);
  }
  env = NULL;

  //释放你的全局引用的接口，生命周期自己把控
  (*env)->DeleteGlobalRef(env, jcallback);
  jcallback = NULL;
}

JavaVM *g_VM;
JNIEXPORT void JNICALL Java_xxx_nativeDownload(
    JNIEnv *env, jobject thiz，jstring jpath，jobject jcallback) {
  // JavaVM是虚拟机在JNI中的表示，等下再其他线程回调java层需要用到
  (*env)->GetJavaVM(env, &g_VM);

  //生成一个全局引用，回调的时候findclass才不会为null
  jobject callback =
      (*env)->NewGlobalRef(env, jcallback)

      // 把接口传进去，或者保存在一个结构体里面的属性， 进行传递也可以
      pthread_create(xxx, xxx, download, callback);
  return;
}