package com.example.smartwatchcompanionapp;

//resources that make the BLE stuff actually work on the android side of things (these were absolutely invalueable)
//https://stackoverflow.com/questions/37181843/android-using-bluetoothgattserver
//https://riptutorial.com/android/example/30768/using-a-gatt-server

import android.app.Service;
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

import static android.bluetooth.le.AdvertiseSettings.ADVERTISE_MODE_BALANCED;
import static android.bluetooth.le.AdvertiseSettings.ADVERTISE_TX_POWER_HIGH;


/*Creates BLE GATT server and handles all data flow through BLE to the
connected device, communication centers here

 */
public class BLEServer extends Service {
    private String TAG = "BLEServer";

    /* UUID's for connecting to the device and accessing this server,
    Recommended that you change these when compiling this yourself. if someone else is
    using this same app and is within range you'll most likely pick up their traffic as well  */
    UUID serviceID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a66");
    UUID characteristicID = UUID.fromString("d3bde760-c538-11ea-8b6e-0800200c9a67");

    //used to split strings up into substrings and send them over BLE as packets
    int currentIndex = 0;

    //bluetooth variables
    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothManager mBluetoothManager;
    private BluetoothLeAdvertiser bluetoothLeAdvertiser;
    private BluetoothGattServer bluetoothGattServer;
    private BluetoothGattService service;
    private BluetoothGattCharacteristic characteristic;
    private BluetoothGattDescriptor descriptor;
    private BluetoothDevice requestingDevice; //device that makes a request command of the android app.


    //create the BLE server
    public void createBLEServer() {
        Log.i(TAG, "BLE Server Starting....");

        //configure Advertise settings, we use TX_POWER_HIGH here with MODE_BALANCED to ensure
        //quick connection to external device, these settings allow for quick connection over BLE
        AdvertiseSettings settings = new AdvertiseSettings.Builder()
                .setConnectable(true)
                .setTxPowerLevel(ADVERTISE_TX_POWER_HIGH)
                .setAdvertiseMode(ADVERTISE_MODE_BALANCED)
                .build();

        AdvertiseData advertiseData = new AdvertiseData.Builder()
                .setIncludeDeviceName(true)
                .setIncludeTxPowerLevel(true)
                .build();

        AdvertiseData scanResponseData = new AdvertiseData.Builder()
                .addServiceUuid(new ParcelUuid(serviceID))
                .setIncludeTxPowerLevel(true)
                .build();

        //get bluetooth manager and access the bluetooth hardware
        mBluetoothManager = getApplicationContext().getSystemService(BluetoothManager.class);
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        //init advertiser with above settings
        bluetoothLeAdvertiser = mBluetoothAdapter.getBluetoothLeAdvertiser();
        bluetoothLeAdvertiser.startAdvertising(settings, advertiseData, scanResponseData, callback);

        //init gatt server with service and characteristic UUIDs
        bluetoothGattServer = mBluetoothManager.openGattServer(getApplicationContext(), gattCallback);
        service = new BluetoothGattService(serviceID, BluetoothGattService.SERVICE_TYPE_PRIMARY);

        //add characteristics and services, these are where the data will be accessed from
        characteristic = new BluetoothGattCharacteristic(characteristicID, BluetoothGattCharacteristic.PROPERTY_READ | BluetoothGattCharacteristic.PROPERTY_WRITE  , BluetoothGattCharacteristic.PERMISSION_READ | BluetoothGattCharacteristic.PERMISSION_WRITE);
        characteristic.setValue("Test3");
        service.addCharacteristic(characteristic);
        bluetoothGattServer.addService(service);
        Log.i(TAG, "BLE Server Created....");
    }

    //establish callbacks, if a BLE device is connected and either writes or reads to this device then it will be forwarded to these
    //callbacks
    BluetoothGattServerCallback gattCallback = new BluetoothGattServerCallback() {

        //Action to do when a device requests a read
        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicReadRequest(device, requestId, offset, characteristic);
            Log.v("reads", "Read Request Received");

            //if the notification data is not ready then we need to inform the other device
            if (!MainActivity.dataIsReady()) {
                //we send a blank string until the data is ready, since the notifications can take a couple of milliseconds to be obtained
                //this is a required step
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "".getBytes());
                Log.v(TAG, "btout: Data Not Ready");
            } else {
                //if the data is ready then we just go ahead and start splitting it up and sending it piece by piece
                //to the external device every time it reads our characteristic
                if(currentIndex <= MainActivity.outData.length()) {
                    try {
                        //send data in 16 character packets
                        bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, MainActivity.outData.substring(currentIndex, currentIndex + 16).getBytes());
                        Log.v(TAG, "BT_OUT: " +  MainActivity.outData.substring(currentIndex, currentIndex + 16));
                    } catch (IndexOutOfBoundsException e) {
                        //if we don't have 16 characters left to send then send whatever is left
                        bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, MainActivity.outData.substring(currentIndex).getBytes());
                        Log.v(TAG, "BT_OUT: " +  MainActivity.outData.substring(currentIndex));
                    }
                }else{
                    //otherwise send asterisks to indicate end of the data
                    bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "********".getBytes());
                    Log.v(TAG, "BT_OUT: " +  "********");
                }
                currentIndex += 16;
            }

        }


        //write request callback, if the device writes to the characteristic then this is the where the device will respond
        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId, BluetoothGattCharacteristic characteristic, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
            super.onCharacteristicWriteRequest(device, requestId, characteristic, preparedWrite, responseNeeded, offset, value);
            String data = new String(value, StandardCharsets.UTF_8);
            requestingDevice = device;
            Log.d(TAG, "BLE triggered notification update via write request, Device Wrote: " + data);

            //each of the conditionals below is a command that can be issued over BLE for this app to handle
            if (data.equals("/notifications")) {
                //get the notification data and put it into our output data field so that the device can read the results
                Log.v(TAG, "BT_OUT: /notifications command received");
                MainActivity.updateText(getApplicationContext());
                currentIndex = 0;
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/currentSong")) {
                //get the song data and put it into the output data field so that the device can read the results, if we're not playing a song
                //then just send the end of string indicator
                Log.v(TAG, "BT_OUT: /currentSong command received");
                if (MainActivity.sReceiver.isPlaying) {
                    MainActivity.outData = MainActivity.reference.sReceiver.getSongData() + "***";
                    MainActivity.reference.setUIText(MainActivity.reference.getApplicationContext(), MainActivity.reference.sReceiver.getSongData());
                } else {
                    MainActivity.outData = "***";
                    MainActivity.reference.setUIText(MainActivity.reference.getApplicationContext(),"***");
                }
                currentIndex = 0;
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/isPlaying")) {
                //put the spotify play status into the output data field so that it can be sent to the device
                Log.v(TAG, "BT_OUT: /isPlaying command received");
                currentIndex = 0;
                MainActivity.outData = MainActivity.reference.sReceiver.isPlaying() + "***";
                MainActivity.reference.setUIText(MainActivity.reference.getApplicationContext(), MainActivity.reference.sReceiver.isPlaying());
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/nextSong")) {
                //just press the next song button when this command is received
                pressMediaKey(KeyEvent.KEYCODE_MEDIA_NEXT);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/lastSong")) {
                //just press the previous song button when this command is received
                pressMediaKey(KeyEvent.KEYCODE_MEDIA_PREVIOUS);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/play")) {
                //just press the play song button when this command is received
                pressMediaKey(KeyEvent.KEYCODE_MEDIA_PLAY);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            } else if (data.equals("/pause")) {
                //just press the pause song button when this command is received
                pressMediaKey(KeyEvent.KEYCODE_MEDIA_PAUSE);
                bluetoothGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, offset, "1".getBytes());
            }
        }
    };

    //advertise callbacks
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

    //just presses a given key on the device, this allows external devices to control the android device
    public void pressMediaKey(int ke){
        //referenced from https://stackoverflow.com/questions/5129027/android-application-to-pause-resume-the-music-of-another-music-player-app
        AudioManager mAudioManager = (AudioManager) MainActivity.reference.getSystemService(Context.AUDIO_SERVICE);
        KeyEvent event = new KeyEvent(KeyEvent.ACTION_DOWN, ke);
        mAudioManager.dispatchMediaKeyEvent(event);
    }

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
