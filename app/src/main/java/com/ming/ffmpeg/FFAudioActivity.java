package com.ming.ffmpeg;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import java.io.File;

public class FFAudioActivity extends AppCompatActivity {

    static {
        System.loadLibrary("ffaudio");
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ffaudio_activity);
        init();
        doOtherThings();
    }

    private final String TAG = "mingzz-ffaudioactivity";

    private void init() {

    }

    private void doOtherThings() {

        PackageManager pm = getPackageManager();
        boolean hasFeature = pm.hasSystemFeature(PackageManager.FEATURE_AUDIO_LOW_LATENCY);
        Log.i(TAG, "doOtherThings: hasFeature FEATURE_AUDIO_LOW_LATENCY : " + hasFeature);

        jniNativeTest();

        initJNINativeCallJava();

        String source = new File(getCacheDir(), "demo.mp4").getAbsolutePath();

        //ffmpeg初始化是耗时的
        new Thread(() -> prepare(source)).start();

    }

    private void progress(){
        Log.i(TAG, "progress: ----------------->");
    }


    public String get(int a) {
        return "jjjjjjjjjjjjjjjjjjjj";
    }

    public native void initJNINativeCallJava();

    public native void prepare(String source);

    public native void jniNativeTest();

}
