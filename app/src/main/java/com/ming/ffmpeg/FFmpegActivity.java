package com.ming.ffmpeg;

import static com.ming.ffmpeg.databinding.ActivityMainBinding.inflate;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import com.ming.ffmpeg.databinding.ActivityMainBinding;

import java.io.File;
import java.util.Arrays;

public class FFmpegActivity extends AppCompatActivity {

    private final String TAG = "mingzzjava";

    // Used to load the 'ffmpeg' library on application startup.
    static {
        System.loadLibrary("ffmpeg");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
        surfaceView = binding.surfaceView;

        init();

    }

    private void init() {
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                Log.i(TAG, "surfaceCreated!");

                surface = holder.getSurface();

                doOtherThings();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                Log.i(TAG, "surfaceChanged!");
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                Log.i(TAG, "surfaceDestroyed!");
            }
        });
    }

    SurfaceView surfaceView;
    Surface surface;

    private void doOtherThings() {
        File cacheDir = getCacheDir();
//        File videoPath = new File(cacheDir, "demo.mp4");
        File videoPath = new File(cacheDir, "fox.mp4");

        play(videoPath.getAbsolutePath(), surface);
    }

    AudioTrack audioTrack;

    public void prepareAudioTrack(int sampleRate, int channels, int encodingFormat) {
        int audioChannelFormat = -1;
        int audioEncodingFormat = -1;
        int streamType = AudioManager.STREAM_MUSIC;
        int transformMode = AudioTrack.MODE_STREAM;
        if (channels < 1 || sampleRate < 1 || encodingFormat < 1) {
            return;
        }
        if (channels == 1) {
            audioChannelFormat = AudioFormat.CHANNEL_OUT_MONO;
        } else if (channels == 2) {
            audioChannelFormat = AudioFormat.CHANNEL_OUT_STEREO;
        } else {
            audioChannelFormat = AudioFormat.CHANNEL_OUT_MONO;
        }
        if (encodingFormat == 8) {
            audioEncodingFormat = AudioFormat.ENCODING_PCM_8BIT;
        } else {
            audioEncodingFormat = AudioFormat.ENCODING_PCM_16BIT;
        }
        int minBufferSize = AudioTrack.getMinBufferSize(sampleRate, audioChannelFormat, audioEncodingFormat);
        Log.i(TAG, "prepareAudioTrack: 通道数,采样率,采样位数,minBufferSize-->"
                + audioChannelFormat
                + "," + sampleRate
                + "," + audioEncodingFormat
                + "," + minBufferSize);
        AudioFormat audioFormat = new AudioFormat.Builder()
                .setChannelMask(audioChannelFormat) //通道数
                .setSampleRate(sampleRate) //采样率
                .setEncoding(audioEncodingFormat) //采样位数
                .build();
        AudioAttributes audioAttributes = new AudioAttributes.Builder()
                .setLegacyStreamType(streamType) //流类型 music alarm..
                .build();
        audioTrack = new AudioTrack.Builder()
                .setAudioFormat(audioFormat)
                .setAudioAttributes(audioAttributes)
                .setTransferMode(transformMode) //传输mode,stream:以流写入播放，static:把数据全部写入播放
                .build();
        audioTrack.play();
    }


    //play audio
    public void audioTrackWrite(byte[] audioPCMBuffer, int length) {
        Log.i(TAG, "audioTrackWrite data-->" + Arrays.toString(audioPCMBuffer));
        if (null != audioTrack && audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
            audioTrack.write(audioPCMBuffer, 0, length);
        }
    }

    public void releaseAudioTrack() {
        if (null != audioTrack) {
            Log.i(TAG, "releaseAudioTrack");
            audioTrack.stop();
            audioTrack.flush();
            audioTrack.release();
        }
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    /**
     * A native method that is implemented by the 'ffmpeg' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void play(String filePath, Surface surface);

    public native void destroy();


}