package com.example.esp32_companion_app;

import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;


import com.harrysoft.androidbluetoothserial.BluetoothManager;
import com.harrysoft.androidbluetoothserial.BluetoothSerialDevice;
import com.harrysoft.androidbluetoothserial.SimpleBluetoothDeviceInterface;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.schedulers.Schedulers;

public class MainActivity extends AppCompatActivity {
    private long lastSendTime = 0;

    public static TextView currentNotification;
    private NLService.NLServiceReceiver nlservicereciver;
    private NotificationReceiver nReceiver;

    public ArrayList<String> notificationHeaders, notificationText;

    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(3);

    //bluetooth connection stuff
    private String deviceAddress;

    private enum Connected {False, Pending, True}

    private Connected connected = Connected.False;

    private SimpleBluetoothDeviceInterface deviceInterface;
    BluetoothManager bluetoothManager;

    //creation method
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        notificationHeaders = new ArrayList();
        notificationText = new ArrayList();

        setContentView(R.layout.activity_main);
        currentNotification = (TextView) findViewById(R.id.CurrentNotificationString);
        currentNotification.setMovementMethod(new ScrollingMovementMethod());

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.kpbird.nlsexample.NOTIFICATION_LISTENER_EXAMPLE");
        registerReceiver(nReceiver, filter);
        getCurrentNotifications();

        Log.i("inform", "App starting....");
        deviceAddress = "24:6F:28:79:E1:6E"; //this is the public address of the esp32 over bluetooth
        Log.i("inform", "Setting device address to fixed value of: " + deviceAddress);

        // Setup our BluetoothManager
        bluetoothManager = BluetoothManager.getInstance();
        if (bluetoothManager == null) {
            // Bluetooth unavailable on this device :( tell the user
            Toast.makeText(getApplicationContext(), "Bluetooth not available.", Toast.LENGTH_LONG).show(); // Replace context with your context instance.
            finish();
        }

        //get list of paired devices
        Collection<BluetoothDevice> pairedDevices = bluetoothManager.getPairedDevicesList();
        for (BluetoothDevice device : pairedDevices) {
            Log.d("test", "Device name: " + device.getName());
            Log.d("test", "Device MAC Address: " + device.getAddress());
            if (device.getName().toLowerCase().equals("ESPWatch".toLowerCase())) {
                Log.d("test", "Found ESP32 watch");
                connectDevice(device.getAddress());
            }
        }
//        deviceInterface.sendMessage(currentNotification.getText().toString());
    }

    private void connectDevice(String mac) {
        bluetoothManager.openSerialDevice(mac)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(this::onConnected, this::onError);
    }

    private void onConnected(BluetoothSerialDevice connectedDevice) {
        // You are now connected to this device!
        // Here you may want to retain an instance to your device:
        deviceInterface = connectedDevice.toSimpleDeviceInterface();

        // Listen to bluetooth events
        deviceInterface.setListeners(this::onMessageReceived, this::onMessageSent, this::onError);

        // Let's send a message:
//        deviceInterface.sendMessage(currentNotification.getText().toString());
        sendNotificationData();
    }

    private void onMessageSent(String message) {
        // We sent a message! Handle it here.
        Toast.makeText(getApplicationContext(), "Sent a message! Message was: " + message, Toast.LENGTH_LONG).show(); // Replace context with your context instance.
    }

    private void onMessageReceived(String message) {
        // We received a message! Handle it here.
        Toast.makeText(getApplicationContext(), "Received a message! Message was: " + message, Toast.LENGTH_LONG).show(); // Replace context with your context instance.
    }

    private void onError(Throwable error) {
        // Handle the error
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        bluetoothManager.close(); //close the bluetooth manager
    }


    //menu items
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main_menu, menu);
        return true;
    }

    //sends notification data over bluetooth
    public void sendNotificationData() {
        //send data in callback of getting the current notifications
        if (lastSendTime + 1000 < System.currentTimeMillis()) {
            lastSendTime = System.currentTimeMillis();
            deviceInterface.sendMessage(currentNotification.getText().toString());
        }
    }

    //updates the onscreen text view with the current notifications active on the device
    public void getCurrentNotifications() {
        currentNotification.setText("");
        notificationHeaders = new ArrayList();
        notificationText = new ArrayList();
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);
    }

    //triggered when an item is selected from the drop down menu
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {
            case R.id.bluetooth:
                // getCurrentNotifications();
                sendNotificationData();
                return true;
            case R.id.update:
                getCurrentNotifications();
                return true;
            case R.id.format:
                currentNotification.setText(reformatNotifications(currentNotification.getText().toString()));
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }


    }

    @Deprecated
    public static String reformatNotifications(String ns) {

        //alright so first we split up the strings and add them to an array list, since we're going to be filtering and sorting these in a kind of weird way
        ArrayList<String> ss = new ArrayList(); //split string

        String mod = ns;
        mod = mod.replace("Notification List", "");
        mod = mod.replace("=", "");
        String[] split = mod.split("\n");

        for (int a = 0; a < split.length; a++) {
            while (split[a].charAt(0) == ' ') {
                split[a] = split[a].substring(1);
            }
        }

        mod = "";
        for (int a = 0; a < split.length; a++) {
            mod += split[a] + "\n";
        }

        //lets see if we can count the indexes
        int highestIndex = 0;
        for (int a = 0; a < split.length; a++) {
            try {
                int currentIndex = Integer.parseInt(split[a].split(" ")[0]);
                if (currentIndex > highestIndex) {
                    highestIndex = currentIndex;
                }
            } catch (Exception e) {

            }
        }

        ArrayList<String> sl = new ArrayList();
        for (int a = 0; a < split.length; a++) {
            sl.add(split[a]);
        }

        String[] formattedNotifications = new String[highestIndex + 1];

        for (int a = 1; a < formattedNotifications.length; a++) {
            //now we need to sort out all the com. objects since these are the "app names"
            int b = 0;
            while (b < sl.size()) {
                String[] currentItem = sl.get(b).split(" ");
                if (Integer.parseInt(currentItem[0]) == a && (currentItem[1].startsWith("com") || currentItem[1].equals("android"))) {
                    formattedNotifications[a] = currentItem[1] + " - ";
                    sl.remove(b);
                }
                b++;
            }
        }

        //before we add the notification text lets get rid of the com. stuff
        for (int a = 0; a < formattedNotifications.length; a++) {
            try {
                formattedNotifications[a] = formattedNotifications[a].replace("com.google.android.gm", "Gmail");
                formattedNotifications[a] = formattedNotifications[a].replace("com.", "");
                //  formattedNotifications[a] = formattedNotifications[a].replace("android", "");
                formattedNotifications[a] = formattedNotifications[a].replace(".", "");
                formattedNotifications[a] = formattedNotifications[a].substring(0, 1).toUpperCase() + formattedNotifications[a].substring(1);
            } catch (Exception e) {
            }
        }

        for (int a = 1; a < formattedNotifications.length; a++) {
            //now we need to sort out all the com. objects since these are the "app names"
            int b = 0;
            while (b < sl.size()) {
                String[] currentItem = sl.get(b).split(" ");
                if (Integer.parseInt(currentItem[0]) == a) {
                    formattedNotifications[a] += sl.get(b).replace(currentItem[0] + " ", "");
                    sl.remove(b);
                }
                b++;
            }
        }


        String finalOutput = "";
        for (int a = 1; a < formattedNotifications.length; a++) {
            finalOutput += formattedNotifications[a] + "\n";
        }


//        return ns + "\n\n-----------------------------------------------------------------------------------------------------------------------\n\n" +
//                mod + "\n\n-----------------------------------------------------------------------------------------------------------------------\n\n"
//                + finalOutput;
        return finalOutput;

    }

    //returns a string representing the date and time
    public static String getDateAndTime() {
        DateTimeFormatter dtf = DateTimeFormatter.ofPattern("MM/dd/yyyy HH:mm:ss");
        LocalDateTime now = LocalDateTime.now();
        return dtf.format(now);
    }

    //class to handle reading notifications from the device.
    class NotificationReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            MainActivity.currentNotification.setText("");

            //first we need to read the number at the start of the message
            String temp = intent.getStringExtra("notification_event");

            try {
                String[] str = temp.split(" ");

                if (str[1].contains("com.") || str[1].contains("android.")) {
                    //so its a header..

                    //figure out it's intended index
                    int index = Integer.parseInt(str[0]);

                    //if the index doesn't exist then make it exist
                    while (notificationHeaders.size() - 1 < index) {
                        notificationHeaders.add("");
                    }

                    notificationHeaders.set(index, temp.substring(str[0].length() + 1));

                } else {

                    //figure out it's intended index
                    int index = Integer.parseInt(str[0]);

                    //if the index doesn't exist then make it exist
                    while (notificationText.size() - 1 < index) {
                        notificationText.add("");
                    }

                    notificationText.set(index, temp.substring(str[0].length() + 1));

                }

                //   String temp = intent.getStringExtra("notification_event") + currentNotification.getText();
                MainActivity.currentNotification.setText(temp);
            } catch (Exception e) {
                //so some notifications won't work. usually they come along with a number that we can use as an index.
                e.printStackTrace();
            }

            String disp = getDateAndTime() + "\n";
            for (int a = 1; a < notificationHeaders.size(); a++) {
                try {
                    if (!notificationHeaders.get(a).equals("") && !notificationText.get(a).equals("")) {
                        try {
                            disp += notificationHeaders.get(a).replace("\n", "");
                            disp += " - ";
                        } catch (Exception e) {
                        }
                        try {
                            disp += notificationText.get(a).replace("\n", "");
                        } catch (Exception e) {
                        }
                        disp += '\n';
                    }
                } catch (IndexOutOfBoundsException ea) {
                }
            }

            //finally replace the titles with the actual app name for some common apps

            disp = disp.replace("com.spotify.music", "Spotify");
            disp = disp.replace("com.google.android.gm", "Gmail");
            disp = disp.replace("com.evernote", "Evernote");
            disp = disp.replace("com.dropbox.android", "Dropbox");
            disp = disp.replace("com.android.systemui", "System");


            MainActivity.currentNotification.setText(disp);
        }
    }


}
