package com.zhaohu.zlog;

import android.os.Looper;

public class Zlog {

    public static int kLevelVerbose = 0;
    public static int kLevelDebug = 1;
    public static int kLevelInfo = 2;
    public static int kLevelWarn = 3;
    public static int kLevelError = 4;
    public static int kLevelFatal = 5;
    public static int kLevelNone = 6;

    //调用so库声明
    static {
        System.loadLibrary("zlog-lib");
    }


    public static void DEBUG(String moudle, String msg) {
        StackTraceElement[] stackTraceElements = Thread.currentThread().getStackTrace();
        String funcName = stackTraceElements[3].getMethodName();
        String fileName = stackTraceElements[3].getFileName();
        int lineNum = stackTraceElements[3].getLineNumber();
        long mypid = android.os.Process.myPid();
        long tid = Thread.currentThread().getId();
        long maintid = Looper.getMainLooper().getThread().getId();

        log(kLevelDebug, moudle, fileName, funcName, lineNum, mypid, tid, maintid, msg);
    }

    public static void INFO(String moudle, String msg) {

        StackTraceElement[] stackTraceElements = Thread.currentThread().getStackTrace();
        String funcName = stackTraceElements[3].getMethodName();
        String fileName = stackTraceElements[3].getFileName();
        int lineNum = stackTraceElements[3].getLineNumber();
        long mypid = android.os.Process.myPid();
        long tid = Thread.currentThread().getId();
        long maintid = Looper.getMainLooper().getThread().getId();

        log(kLevelInfo, moudle, fileName, funcName, lineNum, mypid, tid, maintid, msg);
    }

    public static void WARNING(String moudle, String msg) {
        StackTraceElement[] stackTraceElements = Thread.currentThread().getStackTrace();
        String funcName = stackTraceElements[3].getMethodName();
        String fileName = stackTraceElements[3].getFileName();
        int lineNum = stackTraceElements[3].getLineNumber();
        long mypid = android.os.Process.myPid();
        long tid = Thread.currentThread().getId();
        long maintid = Looper.getMainLooper().getThread().getId();

        log(kLevelWarn, moudle, fileName, funcName, lineNum, mypid, tid, maintid, msg);
    }

    public static void ERROR(String moudle, String msg) {
        StackTraceElement[] stackTraceElements = Thread.currentThread().getStackTrace();
        String funcName = stackTraceElements[3].getMethodName();
        String fileName = stackTraceElements[3].getFileName();
        int lineNum = stackTraceElements[3].getLineNumber();
        long mypid = android.os.Process.myPid();
        long tid = Thread.currentThread().getId();
        long maintid = Looper.getMainLooper().getThread().getId();

        log(kLevelError, moudle, fileName, funcName, lineNum, mypid, tid, maintid, msg);
    }

    public static void FATAL(String moudle, String msg) {
        StackTraceElement[] stackTraceElements = Thread.currentThread().getStackTrace();
        String funcName = stackTraceElements[3].getMethodName();
        String fileName = stackTraceElements[3].getFileName();
        int lineNum = stackTraceElements[3].getLineNumber();
        long mypid = android.os.Process.myPid();
        long tid = Thread.currentThread().getId();
        long maintid = Looper.getMainLooper().getThread().getId();

        log(kLevelFatal, moudle, fileName, funcName, lineNum, mypid, tid, maintid, msg);
    }



    /**
     * native 方法
     */
    public static native void open(String dir, String cachedir, String nameprefix);

    public static native void setLoglevel(int level);

    public static native void enableConsole(Boolean isenable);

    public static native void flush();

    public static native void close();

    public static native void log(int level, String tag, String fileName, String funcName, int lineNum,long pid, long tid, long maintid, String msg);

}
