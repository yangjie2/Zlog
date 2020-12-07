#include <jni.h>
#include <string>
#include <time.h>
#include "src/czlog.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_zhaohu_zlog_Zlog_open(JNIEnv *env, jclass clazz, jstring dir, jstring cachedir,
                               jstring nameprefix) {
    if (NULL == dir || NULL == nameprefix || NULL == cachedir) {
        return;
    }
    const char *log_dir = env->GetStringUTFChars(dir,0);
    const char *cache_dir = env->GetStringUTFChars(cachedir, 0);
    const char *name_prefix = env->GetStringUTFChars(nameprefix, 0);
    c_logopen(log_dir, cache_dir, name_prefix, "");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zhaohu_zlog_Zlog_setLoglevel(JNIEnv *env, jclass clazz, jint level) {
    c_setloglevel((ZlogLevel)level);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zhaohu_zlog_Zlog_enableConsole(JNIEnv *env, jclass clazz, jobject isenable) {
    // TODO: implement enableConsole()

}

extern "C"
JNIEXPORT void JNICALL
Java_com_zhaohu_zlog_Zlog_flush(JNIEnv *env, jclass clazz) {
    c_logflush();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zhaohu_zlog_Zlog_close(JNIEnv *env, jclass clazz) {
    c_logclose();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zhaohu_zlog_Zlog_log(JNIEnv *env, jclass clazz, jint level, jstring tag, jstring file_name,
                              jstring func_name, jint line_num, jlong pid, jlong tid, jlong maintid,
                              jstring msg) {
    const char *jni_module_tag = env->GetStringUTFChars(tag, 0);
    const char *jni_message = env->GetStringUTFChars(msg, 0);
    const char *jni_file_name = env->GetStringUTFChars(file_name, 0);
    const char *jni_func_name = env->GetStringUTFChars(func_name, 0);
    ZLogBasicInfo info;
    info.level = (ZlogLevel)level;
    info.tag = jni_module_tag;
    info.filename = jni_file_name;
    info.line = line_num;
    info.func_name = jni_func_name;
    info.pid = pid;
    info.tid = tid;
    info.main_tid = maintid;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    info.timeval = tv;
    c_writelog(&info, jni_message);
}