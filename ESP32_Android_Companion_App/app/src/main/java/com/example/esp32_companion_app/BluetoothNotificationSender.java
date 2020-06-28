package com.example.esp32_companion_app;

import android.app.job.JobParameters;
import android.app.job.JobService;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.util.Log;
//import android.widget.Toast;

import com.harrysoft.androidbluetoothserial.BluetoothManager;
import com.harrysoft.androidbluetoothserial.BluetoothSerialDevice;

import java.util.Collection;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.schedulers.Schedulers;

/*  Using Harry1453's android-bluetooth serial library to simplify bluetooth communication
    https://github.com/harry1453/android-bluetooth-serial */
import com.harrysoft.androidbluetoothserial.BluetoothManager;
import com.harrysoft.androidbluetoothserial.BluetoothSerialDevice;
import com.harrysoft.androidbluetoothserial.SimpleBluetoothDeviceInterface;



public class BluetoothNotificationSender extends JobService {

    private SimpleBluetoothDeviceInterface deviceInterface;
    BluetoothManager bluetoothManager;

    @Override
    public boolean onStartJob(JobParameters params) {
//        Toast.makeText(this, "Starting the Bluetooth Task", Toast.LENGTH_SHORT).show();

        try{
            connectToWatch();
        }catch(Exception e){
            Log.e("inform", "Could not connect to watch");
        }


        return false;
    }

    @Override
    public boolean onStopJob(JobParameters params) {
        deviceInterface = null;
        bluetoothManager.close();
        bluetoothManager = null;
        return false;
    }



    private void connectToWatch() {
        // Setup our BluetoothManager
        bluetoothManager = BluetoothManager.getInstance();
        if (bluetoothManager == null) {
            // Bluetooth unavailable on this device :( tell the user
//            Toast.makeText(getApplicationContext(), "Bluetooth not available.", Toast.LENGTH_LONG).show(); // Replace context with your context instance.
//            finish();
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
//        sendNotificationData();
        deviceInterface.sendMessage(MainActivity.currentNotificationString + "*"); //send the notification * marks end of data
    }

    private void onMessageSent(String message) {
        // We sent a message! Handle it here.
//        Toast.makeText(getApplicationContext(), "Sent a message! Message was: " + message, Toast.LENGTH_LONG).show(); // Replace context with your context instance.
    }

    private void onMessageReceived(String message) {
        // We received a message! Handle it here.
//        Toast.makeText(getApplicationContext(), "Received a message! Message was: " + message, Toast.LENGTH_LONG).show(); // Replace context with your context instance.
    }

    private void onError(Throwable error) {
        // Handle the error
    }




}
