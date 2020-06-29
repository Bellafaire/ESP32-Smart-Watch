package com.example.smartwatchcompanionapp;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MainActivity extends Activity {

    public static TextView txtView ;
    public static TextView status;
    private Button settings, forceSend;
    private Button btn;
    private NotificationReceiver nReceiver;

    public ArrayList<String> notificationHeaders, notificationText;

    String TAG = "inform";

  public static  Handler handler = new Handler();

    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
    private final ExecutorService threads = Executors.newCachedThreadPool();

    BluetoothNotificationSender bns;

    private Runnable notificationUpdater = new Runnable() {
        @Override
        public void run() {
            try {
                updateText();
            } catch (Exception e) {
            }
        }
    };

    private Runnable sendBT = new Runnable() {
        @Override
        public void run() {

            handler.post(new Runnable(){
                public void run() {
                    status.setText("Status: Sending Bluetooth Data");
                }
            });

            Log.d("bt", "attempting to send bluetooth data");
            bns = new BluetoothNotificationSender();
            bns.connectToWatch();
//            bns.end();

            handler.post(new Runnable(){
                public void run() {
                    status.setText("Status: Last Sent Data at " + getDateAndTime());
                }
            });


        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        notificationHeaders = new ArrayList();
        notificationText = new ArrayList();

        Log.i(TAG, "application starting");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        status   = (TextView) findViewById(R.id.statusInformationBar);
        txtView = (TextView) findViewById(R.id.txt);
        btn = (Button) findViewById(R.id.button);
        settings = (Button) findViewById(R.id.settings);
        settings = (Button) findViewById(R.id.sendButton);

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.kpbird.nlsexample.NOTIFICATION_LISTENER_EXAMPLE");

        registerReceiver(nReceiver, filter);

        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);

        Log.i("inform", "App starting....");
        scheduler.scheduleAtFixedRate(notificationUpdater, 0, 30, TimeUnit.SECONDS);
        Log.i("inform", "scheduled notification updater");
    }

    public void sendBluetoothData() {

        threads.execute(sendBT);

    }


    public void forceSend(View view) {
        sendBluetoothData();
    }


    public void gotoSettings(View view) {
        Intent intent = new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS");
        startActivity(intent);
    }

    public void updateText() {
        Log.d(TAG, "update text function has been called");
        txtView.setText( getDateAndTime() + "\n***");
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);
    }

    public void updateText(View view) {
        Log.i(TAG, "update text button has been pressed");
        txtView.setText( getDateAndTime() + "\n***");
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(nReceiver);
        bns.end();
    }


    //returns a string representing the date and time
    public static String getDateAndTime() {
        Date c = Calendar.getInstance().getTime();
        System.out.println("Current time => " + c);

        SimpleDateFormat df = new SimpleDateFormat("hh:mm:ssa dd-MM-yyyy");
        String formattedDate = df.format(c);
        return formattedDate;

    }

    class NotificationReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "onRecieve method callback received ");
            String temp = intent.getStringExtra("notification_event") + "\n" + txtView.getText();
            txtView.setText(temp.replace("\n\n", "\n"));
            if (txtView.getText().toString().contains("***")) {
                sendBluetoothData();
            }

        }
    }


}
