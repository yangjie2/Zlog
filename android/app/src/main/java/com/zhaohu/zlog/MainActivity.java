package com.zhaohu.zlog;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.os.Looper;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String logdir = getCachePath(this)+"/log";
        Zlog.open(logdir, logdir+"/logcache", "1234");

        long startTime = System.currentTimeMillis();    //获取开始时间
        for (int ii = 0; ii<100000; ii++) {
            Zlog.INFO("zhoahu", "换地方撒的；书法家杜萨分卡大街上疯狂；安静发阿打开手机发放开；打算减肥卡的；将饭卡时间放开；阿减肥打卡时间放开；爱睡觉放开就撒开发多少啊发觉卡技术开发健康的九分裤阔脚裤；dadfas");
        }
        Zlog.flush();
        long endTime = System.currentTimeMillis();    //获取结束时间
        System.out.println("运行时间：" + (endTime - startTime)/1000. + " s");
    }

    private String getCachePath(Context context) {
        String cachePath = null;
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())
                || !Environment.isExternalStorageRemovable()) {
            cachePath = context.getExternalCacheDir().getPath();
        } else {
            cachePath = context.getCacheDir().getPath();
        }
        return cachePath;
    }


}