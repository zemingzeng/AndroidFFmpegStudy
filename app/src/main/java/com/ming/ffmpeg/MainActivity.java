package com.ming.ffmpeg;

import static com.ming.ffmpeg.databinding.ActivityMainBinding.*;

import android.security.keystore.KeyProperties;
import android.security.keystore.KeyProtection;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.ming.ffmpeg.databinding.ActivityMainBinding;

import javax.crypto.spec.SecretKeySpec;

public class MainActivity extends AppCompatActivity {

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

        new Thread() {
            @Override
            public void run() {
                super.run();
                binding.getRoot();
            }
        };
    }

    /**
     * A native method that is implemented by the 'ffmpeg' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void play(String filePath);


    int getNumber() {
        return 10;
    }

    void play() {
        int yy[] = new int[getNumber()];
        int uu[];
        uu = new int[111];
    }
}