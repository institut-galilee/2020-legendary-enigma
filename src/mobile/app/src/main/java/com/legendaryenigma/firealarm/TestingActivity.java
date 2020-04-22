package com.legendaryenigma.firealarm;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

public class TestingActivity extends AppCompatActivity {


    private static final int REQUEST_ENABLE_BT = 1;
    private static final String DEVICE_NAME = "FireAlarm_LE";
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1538-8008-00803e4f5a6b");

    CheckBox cbSmoke, cbGas, cbSms, cbNotification;
    Button btnTest;

    BluetoothAdapter bta;
    BluetoothSocket mmSocket;
    BluetoothDevice mmDevice;
    BluetoothThread btt = null;
    Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_testing);

        cbSmoke = (CheckBox) findViewById(R.id.cb_test_smoke);
        cbGas = (CheckBox) findViewById(R.id.cb_test_gas);
        cbSms = (CheckBox) findViewById(R.id.cb_test_sms);
        cbNotification = (CheckBox) findViewById(R.id.cb_test_notification);
        btnTest = (Button) findViewById(R.id.btn_test);

        btnTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i("[BLUETOOTH]", "Attempting to send data");
                try {
                    if (mmSocket.isConnected() && btt != null) {
                        String testingData = "";
                        testingData += cbSmoke.isChecked() ? "1":"2";
                        testingData += cbGas.isChecked() ? "1":"2";
                        testingData += cbSms.isChecked() ? "1":"2";
                        testingData += cbNotification.isChecked() ? "1":"2";
                        String msgTestingData = "105:" + testingData + "==";
                        showToast("message testing data " + msgTestingData);
                        btt.write(msgTestingData.getBytes());
                    } else {
                        initiateBluetooth();
                    }
                } catch (Exception ex) {
                    Log.e("[ERROR]", ex.getMessage());
                    showToast(ex.getMessage());
                }
            }
        });
    }


    public void initiateBluetooth() {

        if (bta.isEnabled()) {
            //attempt to connect to bluetooth module
            BluetoothSocket tmp = null;
            String deviceAddress = null;
            Set<BluetoothDevice> pairedDevices = BluetoothAdapter.getDefaultAdapter().getBondedDevices();
            if (pairedDevices.size() > 0) {
                for (BluetoothDevice d : pairedDevices) {
                    String deviceName = d.getName();
                    if (deviceName.equals(DEVICE_NAME)) {
                        deviceAddress = d.getAddress();
                        showToast("paired device: " + deviceName + " at " + deviceAddress);
                    }
                    String macAddress = d.getAddress();
                    Log.i("[BLUETOOTH]", "paired device: " + deviceName + " at " + macAddress);
                    // do what you need/want this these list items
                }
            }
            if (deviceAddress != null) {
                mmDevice = bta.getRemoteDevice(deviceAddress);

                //create socket
                try {
                    tmp = mmDevice.createRfcommSocketToServiceRecord(MY_UUID);
                    mmSocket = tmp;
                    mmSocket.connect();
                    Log.i("[BLUETOOTH]", "Connected to: " + mmDevice.getName());
                } catch (IOException e) {
                    showToast(e.getMessage());
                    try {
                        mmSocket.close();
                    } catch (IOException c) {
                        showToast(c.getMessage());
                        return;
                    }
                }

                Log.i("[BLUETOOTH]", "Creating handler");
                Handler mHandler = new Handler(Looper.getMainLooper()) {
                    @Override
                    public void handleMessage(Message msg) {
                        //super.handleMessage(msg);
                        if (msg.what == BluetoothThread.RESPONSE_MESSAGE) {
                            String txt = (String) msg.obj;
                            Log.i("[BLUETOOTH]", "Response = " + txt);
                        }
                    }
                };

                Log.i("[BLUETOOTH]", "Creating and running Thread");
                btt = new BluetoothThread(mmSocket, mHandler);
                btt.start();
            } else {
                showToast("You have to connect to the device by Bluetooth at least one time");
            }
        }
    }
    public void showToast(final String Text) {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(TestingActivity.this,
                        Text, Toast.LENGTH_LONG).show();
            }
        });
    }
}