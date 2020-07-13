package com.example.smartwatchcompanionapp;

//resources that make the BLE stuff actually work on the android side of things (these were absolutely invalueable)
//https://stackoverflow.com/questions/37181843/android-using-bluetoothgattserver
//https://riptutorial.com/android/example/30768/using-a-gatt-server

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.UUID;

public class MainActivity extends Activity {

    UUID serviceID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a66");
    UUID characteristicID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a67");
    UUID descriptorID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a68");

    long lastReadRequest = 0;
    int currentIndex = 0;

    public static TextView txtView;
    public static TextView status;
    private Button settings, forceSend;
    private Button btn;
    private NotificationReceiver nReceiver;

    //    private String deviceAddress = "24:6F:28:81:82:4E"; //watch address
//    private String deviceAddress = "24:6F:28:79:E1:6E"; //esp32 test board

    private TextView messages;

    public ArrayList<String> notificationHeaders, notificationText;

    String TAG = "inform";

    public static Handler handler = new Handler();
    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothManager mBluetoothManager;
    private BluetoothLeAdvertiser bluetoothLeAdvertiser;
    private BluetoothGattServer bluetoothGattServer;
    private BluetoothGattService service;
    private BluetoothGattCharacteristic characteristic;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
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
        settings = (Button) findViewById(R.id.sendButton);
        messages = (TextView) findViewById(R.id.messages);
        messages.append("\n");

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.kpbird.nlsexample.NOTIFICATION_LISTENER_EXAMPLE");
        registerReceiver(nReceiver, filter);

        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);

        Log.i("inform", "App starting....");

        AdvertiseSettings settings = new AdvertiseSettings.Builder()
                .setConnectable(true)
                .build();

        AdvertiseData advertiseData = new AdvertiseData.Builder()
                .setIncludeDeviceName(true)
                .setIncludeTxPowerLevel(true)
                .build();

        AdvertiseData scanResponseData = new AdvertiseData.Builder()
                .addServiceUuid(new ParcelUuid(serviceID))
                .setIncludeTxPowerLevel(true)
                .build();


        mBluetoothManager = getApplicationContext().getSystemService(BluetoothManager.class);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        bluetoothLeAdvertiser = mBluetoothAdapter.getBluetoothLeAdvertiser();
        bluetoothLeAdvertiser.startAdvertising(settings, advertiseData, scanResponseData, callback);

        bluetoothGattServer = mBluetoothManager.openGattServer(getApplicationContext(), gattCallback);
        service = new BluetoothGattService(serviceID, BluetoothGattService.SERVICE_TYPE_PRIMARY);

//add a read characteristic.
        characteristic = new BluetoothGattCharacteristic(characteristicID, BluetoothGattCharacteristic.PROPERTY_READ, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);
        characteristic.setValue("Test3");
        service.addCharacteristic(characteristic);
        bluetoothGattServer.addService(service);

        //        bluetoothGattServer.getService(serviceID).getCharacteristic(characteristicID).setValue("Test3");
    }

    BluetoothGattServerCallback gattCallback = new BluetoothGattServerCallback() {
        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicReadRequest(device, requestId, offset, characteristic);
            try {
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, txtView.getText().toString().substring(currentIndex, currentIndex + 16).getBytes());
                Log.v("btout", "BT_OUT:" + txtView.getText().toString().substring(currentIndex, currentIndex + 16).getBytes());
            } catch (IndexOutOfBoundsException e) {
                if (currentIndex < txtView.getText().toString().length()) {
                    String res = txtView.getText().toString().substring(currentIndex);
                    while (res.length() < 16) {
                        res += "*";
                    }
                    bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, res.getBytes());
                    Log.v("btout", "BT_OUT:" + res);
                } else {
                    bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "****".getBytes());
                    Log.v("btout", "BT_OUT:" + "****");
                }

            }
            currentIndex += 16;

        }

        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId, BluetoothGattCharacteristic characteristic, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
            super.onCharacteristicWriteRequest(device, requestId, characteristic, preparedWrite, responseNeeded, offset, value);

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    updateText();
                }
            });

            Log.d("inform", "BLE triggered notification update via write request");
            currentIndex = 0;
            bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "****".getBytes());
        }

    };

    AdvertiseCallback callback = new AdvertiseCallback() {
        @Override
        public void onStartSuccess(AdvertiseSettings settingsInEffect) {
            Log.d(TAG, "BLE advertisement added successfully");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    messages.append("BLE Advertisements Established - " + getDateAndTime() + "\n");
                }
            });
        }

        @Override
        public void onStartFailure(int errorCode) {
            Log.e(TAG, "Failed to add BLE advertisement, reason: " + errorCode);
        }
    };

    //only used by the force BT send button
    public void sendBluetoothData() {

    }


    private void status(String str) {
        messages.append(str + "\n");
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
        messages.append("Last Update Sent: " + getDateAndTime() + "\n");
        txtView.setText(getDateAndTime() + "\n***");
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);
    }

    public void updateText(View view) {
        Log.i(TAG, "update text button has been pressed");
        messages.append("Last Update Sent: " + getDateAndTime() + "\n");
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

        SimpleDateFormat df = new SimpleDateFormat("hh:mm:ssa dd-MM-yyyy");
        String formattedDate = df.format(c);
        return formattedDate;

    }

    class NotificationReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "onRecieve method callback received " + intent.getStringExtra("notification_event"));
            String temp = intent.getStringExtra("notification_event") + "\n" + txtView.getText();
            txtView.setText(temp.replace("\n\n", "\n"));
        }
    }

}
