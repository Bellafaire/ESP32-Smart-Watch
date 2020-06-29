package com.example.smartwatchcompanionapp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

    private TextView txtView;
    private Button settings;
    private Button btn;
    private NotificationReceiver nReceiver;

    String TAG = "inform";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "application starting");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txtView= (TextView) findViewById(R.id.txt);
        btn = (Button) findViewById(R.id.button);
        settings = (Button) findViewById(R.id.settings);

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.kpbird.nlsexample.NOTIFICATION_LISTENER_EXAMPLE");

       registerReceiver(nReceiver,filter);

        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command","list");
        sendBroadcast(i);

    }


    public void gotoSettings(View view){
        Intent intent=new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS");
        startActivity(intent);
    }


    public void updateText(View view){
        Log.i(TAG, "update text button has been pressed");
        txtView.setText("");
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command","list");
        sendBroadcast(i);

    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(nReceiver);
    }

    class NotificationReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "onRecieve method callback received ");
            String temp = intent.getStringExtra("notification_event") + "\n" + txtView.getText();
            txtView.setText(temp);
        }
    }




}
