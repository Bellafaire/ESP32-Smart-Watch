package com.example.smartwatchcompanionapp;

//resources that make the BLE stuff actually work on the android side of things (these were absolutely invalueable)
//https://stackoverflow.com/questions/37181843/android-using-bluetoothgattserver
//https://riptutorial.com/android/example/30768/using-a-gatt-server

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

public class MainActivity extends Activity {

    public static boolean bleServiceRunning = false;
    public static String outData = "";
    public static String logData = "Messages\n";

    public static TextView txtView;
    public static TextView status;
    private Button settings, toggleBT;
    private Button btn;
    private NotificationReceiver nReceiver;
    public static SpotifyReceiver sReceiver;

    public static TextView messages;
    public static MainActivity reference;

    public ArrayList<String> notificationHeaders, notificationText;

    String TAG = "inform";

    public static Handler handler = new Handler();

    BLEServer ble;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        reference = this;
        notificationHeaders = new ArrayList();
        notificationText = new ArrayList();

        Log.i(TAG, "application starting...");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        status = (TextView) findViewById(R.id.statusInformationBar);
        status.setMovementMethod(new ScrollingMovementMethod());
        txtView = (TextView) findViewById(R.id.txt);
        btn = (Button) findViewById(R.id.button);
        settings = (Button) findViewById(R.id.settings);
        toggleBT = (Button) findViewById(R.id.sendButton);
        messages = (TextView) findViewById(R.id.messages);
        messages.append("\n");

        sReceiver = new SpotifyReceiver();
        IntentFilter sfilter = new IntentFilter();
        sfilter.addAction("com.spotify.music.playbackstatechanged");
        sfilter.addAction("com.spotify.music.metadatachanged");
        sfilter.addAction("com.spotify.music.queuechanged");
        registerReceiver(sReceiver, sfilter);


        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.kpbird.nlsexample.NOTIFICATION_LISTENER_EXAMPLE");
        registerReceiver(nReceiver, filter);

        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);

        startService(new Intent(MainActivity.this, BLEServer.class));
    }

    //only used by the force BT send button
    public void sendBluetoothData() {
        Toast.makeText(getApplicationContext(), "sendBluetoothData() disabled (for now)",
                Toast.LENGTH_SHORT).show();
    }


    private void status(String str) {
        messages.append(str + "\n");
    }

    public static void updateBLEStatus() {
        reference.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (bleServiceRunning) {
                    reference.toggleBT.setText("Disable BLE Service");
                } else {
                    reference.toggleBT.setText("Enable BLE Service");
                }
            }
        });
    }

    public void toggleService(View view) {
        if (bleServiceRunning) {
            stopService(new Intent(MainActivity.this, BLEServer.class));
        } else {
            startService(new Intent(MainActivity.this, BLEServer.class));
        }
    }


    public void gotoSettings(View view) {
        Intent intent = new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS");
        startActivity(intent);
    }

    public void updateText() {
        Log.d("inform", "update text function has been called");

        outData = getDateAndTime() + "\n***";
        txtView.setText(getDateAndTime() + "\n***");
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);

//        messages.append("BLE Data Sent - " + getDateAndTime() + "\n");
        logData += "BLE Data Sent - " + getDateAndTime() + "\n";
        messages.setText(logData);
    }


    public static void updateText(Context context) {
        Log.d("inform", "update text function has been called");
        reference.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                outData = getDateAndTime() + "\n***";
                txtView.setText(getDateAndTime() + "\n***");
                Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
                i.putExtra("command", "list");
                context.sendBroadcast(i);
//                messages.setText("BLE Data Sent - " + getDateAndTime() + "\n");
                logData += "BLE Data Sent - " + getDateAndTime() + "\n";
                messages.setText(logData);
            }
        });
    }

    public static void setUIText(Context context, String data) {
        Log.d("inform", "update text function has been called");
        reference.runOnUiThread(new Runnable() {
            @Override
            public void run() {

                txtView.setText(data + "***");
                messages.setText(logData);
            }
        });
    }


    @Override
    protected void onResume() {
        super.onResume();
        messages.setText(logData);
    }

    public void updateText(View view) {
        Log.i(TAG, "update text button has been pressed");
//        messages.append("Manually updated: " + getDateAndTime() + "\n");
        logData += "Manually updated: " + getDateAndTime() + "\n";
        messages.setText(logData);
        outData = getDateAndTime() + "\n***";
        txtView.setText(getDateAndTime() + "\n***");
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(nReceiver);
    }


    //returns a string representing the date and time
    public static String getDateAndTime() {
        Date c = Calendar.getInstance().getTime();
        System.out.println("Current time => " + c);

        SimpleDateFormat df = new SimpleDateFormat("HH:mm:ss dd-MM-yyyy");
        String formattedDate = df.format(c);
        return formattedDate;

    }

    class NotificationReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "onRecieve method callback received " + intent.getStringExtra("notification_event"));
            String temp = intent.getStringExtra("notification_event");
            if (!outData.contains(temp)) {
                temp = intent.getStringExtra("notification_event") + "\n" + outData;
                outData = temp.replace("\n\n", "\n");
            }
            if (!txtView.getText().toString().contains(temp)) {
                temp = intent.getStringExtra("notification_event") + "\n" + txtView.getText();
                txtView.setText(temp.replace("\n\n", "\n"));
            }
//            String temp = intent.getStringExtra("notification_event") + "\n" + txtView.getText();

        }
    }


}
