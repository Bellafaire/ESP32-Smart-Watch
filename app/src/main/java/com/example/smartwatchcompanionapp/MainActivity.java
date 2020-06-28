package com.example.smartwatchcompanionapp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

    private TextView txtView;
    private Button btn;
    private NotificationReceiver nReceiver;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txtView= (TextView) findViewById(R.id.txt);
        btn = (Button) findViewById(R.id.button);

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.kpbird.nlsexample.NOTIFICATION_LISTENER_EXAMPLE");
        registerReceiver(nReceiver,filter);

        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command","list");
        sendBroadcast(i);
    }


    public void updateText(View view){
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
            String temp = intent.getStringExtra("notification_event") + "\n" + txtView.getText();
            txtView.setText(temp);
        }
    }




}
