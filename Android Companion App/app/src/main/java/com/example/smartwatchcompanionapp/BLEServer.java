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
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.os.IBinder;
import android.os.ParcelUuid;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;

import androidx.annotation.Nullable;

import java.nio.charset.StandardCharsets;
import java.util.UUID;

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
        characteristic = new BluetoothGattCharacteristic(characteristicID, BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE, BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);
        characteristic.setValue("Test3");
        service.addCharacteristic(characteristic);
        bluetoothGattServer.addService(service);
        Log.i(TAG, "BLE Server Created....");
    }

    BluetoothGattServerCallback gattCallback = new BluetoothGattServerCallback() {
        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicReadRequest(device, requestId, offset, characteristic);
            Log.v("reads", "Read Request Received");
            //if the notification data is not ready then we need to inform the other device
            if (!MainActivity.outData.contains("***") || currentIndex < 0 ) {
                //send the word "null" until the notification data is available
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "null".getBytes());
                Log.v(TAG, "btout: Data Not Ready");
            } else {
                if (MainActivity.outData.length() < 16 && currentIndex == 0) {
                    bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, MainActivity.outData.getBytes());
                    Log.v("btout", "BT_OUT:" + MainActivity.outData.getBytes(StandardCharsets.UTF_8));
                } else {
                    try {
                        bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, MainActivity.outData.substring(currentIndex, currentIndex + 16).getBytes());
                        Log.v("btout", "BT_OUT:" + MainActivity.outData.substring(currentIndex, currentIndex + 16).getBytes(StandardCharsets.UTF_8));
                    } catch (IndexOutOfBoundsException e) {
                        if (currentIndex < MainActivity.outData.length()) {
                            String res = MainActivity.outData.substring(currentIndex);
                            while (res.length() < 16) {
                                res += "*";
                            }
                            bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, res.getBytes());
                            Log.v("btout", "BT_OUT:" + res.getBytes(StandardCharsets.UTF_8));
                        } else {
                            bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "****".getBytes());
                            Log.v("btout", "BT_OUT:" + "****");
                        }

                    }
                }

            }
            currentIndex += 16;
        }

        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId, BluetoothGattCharacteristic characteristic, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
            super.onCharacteristicWriteRequest(device, requestId, characteristic, preparedWrite, responseNeeded, offset, value);
            String data = new String(value, StandardCharsets.UTF_8);
            Log.d(TAG, "BLE triggered notification update via write request, Device Wrote: " + data);
            if (data.equals("/notifications")) {
                Log.v(TAG, "BT_OUT: /notifications command received");
                MainActivity.updateText(getApplicationContext());
                currentIndex = 0;
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/currentSong")) {
                Log.v(TAG, "BT_OUT: /currentSong command received");
                if (MainActivity.sReceiver.isPlaying) {
                    MainActivity.outData = MainActivity.reference.sReceiver.getSongData() + "***";
                    MainActivity.reference.setUIText(MainActivity.reference.getApplicationContext(), MainActivity.reference.sReceiver.getSongData());
                } else {
                    MainActivity.outData = "***";
                    MainActivity.reference.setUIText(MainActivity.reference.getApplicationContext(),"***");
                }
                currentIndex = -32;
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/isPlaying")) {
                Log.v(TAG, "BT_OUT: /isPlaying command received");
                currentIndex = 0;
                MainActivity.outData = MainActivity.reference.sReceiver.isPlaying() + "***";
                MainActivity.reference.setUIText(MainActivity.reference.getApplicationContext(), MainActivity.reference.sReceiver.isPlaying());
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/nextSong")) {
                //referenced from https://stackoverflow.com/questions/5129027/android-application-to-pause-resume-the-music-of-another-music-player-app
                AudioManager mAudioManager = (AudioManager) MainActivity.reference.getSystemService(Context.AUDIO_SERVICE);
                KeyEvent event = new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_MEDIA_NEXT);
                mAudioManager.dispatchMediaKeyEvent(event);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/lastSong")) {
                //referenced from https://stackoverflow.com/questions/5129027/android-application-to-pause-resume-the-music-of-another-music-player-app
                AudioManager mAudioManager = (AudioManager) MainActivity.reference.getSystemService(Context.AUDIO_SERVICE);
                KeyEvent event = new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_MEDIA_PREVIOUS);
                mAudioManager.dispatchMediaKeyEvent(event);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/play")) {
                //referenced from https://stackoverflow.com/questions/5129027/android-application-to-pause-resume-the-music-of-another-music-player-app
                AudioManager mAudioManager = (AudioManager) MainActivity.reference.getSystemService(Context.AUDIO_SERVICE);
                KeyEvent event = new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_MEDIA_PLAY);
                mAudioManager.dispatchMediaKeyEvent(event);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/pause")) {
                //referenced from https://stackoverflow.com/questions/5129027/android-application-to-pause-resume-the-music-of-another-music-player-app
                AudioManager mAudioManager = (AudioManager) MainActivity.reference.getSystemService(Context.AUDIO_SERVICE);
                KeyEvent event = new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_MEDIA_PAUSE);
                mAudioManager.dispatchMediaKeyEvent(event);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            }
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

    @Override
    public void onDestroy() {
        super.onDestroy();
        Toast.makeText(this, "BLE Service Has Stopped", Toast.LENGTH_SHORT).show();
        MainActivity.bleServiceRunning = false;
        MainActivity.updateBLEStatus();
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "BLEServer Object Created");
        createBLEServer();
        Toast.makeText(this, "BLE Service Has Started", Toast.LENGTH_SHORT).show();
        MainActivity.bleServiceRunning = true;
        MainActivity.updateBLEStatus();
        return Service.START_STICKY;
    }

}
