package com.example.smartwatchcompanionapp;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.content.Intent;
import android.os.IBinder;
import android.os.ParcelUuid;
import android.util.Log;

import androidx.annotation.Nullable;

import java.util.UUID;

import static com.example.smartwatchcompanionapp.MainActivity.messages;
import static com.example.smartwatchcompanionapp.MainActivity.txtView;

public class BLEServer extends Service {
    private String TAG = "BLEServer";
    public static final String CHANNEL_ID = "ForegroundServiceChannel";

    UUID serviceID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a66");
    UUID characteristicID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a67");
    UUID descriptorID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a68");

    int currentIndex = 0;

    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothManager mBluetoothManager;
    private BluetoothLeAdvertiser bluetoothLeAdvertiser;
    private BluetoothGattServer bluetoothGattServer;
    private BluetoothGattService service;
    private BluetoothGattCharacteristic characteristic;


    public void createBLEServer() {
        Log.i(TAG, "BLE Server Starting....");
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
        Log.i(TAG, "BLE Server Created....");
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
            MainActivity.updateText(getApplicationContext());
            Log.d(TAG, "BLE triggered notification update via write request");
            currentIndex = 0;
            bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "****".getBytes());
        }

    };

    AdvertiseCallback callback = new AdvertiseCallback() {
        @Override
        public void onStartSuccess(AdvertiseSettings settingsInEffect) {
            Log.d(TAG, "BLE advertisement added successfully");
        }

        @Override
        public void onStartFailure(int errorCode) {
            Log.e(TAG, "Failed to add BLE advertisement, reason: " + errorCode);
        }
    };

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        createBLEServer();
        return null;
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "BLEServer Object Created");
        createBLEServer();

        return Service.START_NOT_STICKY;
    }

}