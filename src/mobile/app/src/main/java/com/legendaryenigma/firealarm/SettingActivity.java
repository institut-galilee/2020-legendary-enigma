package com.legendaryenigma.firealarm;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

public class SettingActivity extends AppCompatActivity {

    private static final int REQUEST_ENABLE_BT = 1;
    private static final String DEVICE_NAME = "FireAlarm_LE";
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1538-8008-00803e4f5a6b");
    EditText etWifiName, etWifiPass, etPhone, etPhoneAgency;
    Button btnUpdateWifi, btnUpdatePhone;

    BluetoothAdapter bta;
    BluetoothSocket mmSocket;
    BluetoothDevice mmDevice;
    BluetoothThread btt = null;
    Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);

        etWifiName = (EditText) findViewById(R.id.et_wifi);
        etWifiPass = (EditText) findViewById(R.id.et_pass);
        etPhone = (EditText) findViewById(R.id.et_phone);
        etPhoneAgency = (EditText) findViewById((R.id.et_phone_agency));

        btnUpdateWifi = (Button) findViewById(R.id.btn_wifi_update);
        btnUpdatePhone = (Button) findViewById(R.id.btn_phone_update);

        btnUpdateWifi.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i("[BLUETOOTH]", "Attempting to send data");
                try {
                    if (mmSocket.isConnected() && btt != null) {
                        Editable wifiName = etWifiName.getText();
                        Editable wifiPass = etWifiPass.getText();
                        showToast("wifi info: " + wifiName + wifiPass + wifiName.length() + wifiPass.length());
                        if (wifiName.length() > 0 && wifiPass.length() >= 8) {
                            String msgWifiName = "101:" + wifiName.toString() + "==";
                            Log.d("[BLUETOOTH]", msgWifiName);
                            btt.write(msgWifiName.getBytes());
                            String msgWifiPass = "102:" + wifiPass.toString() + "==";
                            Log.d("[BLUETOOTH]", msgWifiPass);
                            showToast("msg wifi info:" + msgWifiName + msgWifiPass);
                            btt.write(msgWifiPass.getBytes());
                        } else {
                            showToast("The shortest password allowed is 8 characters long.");
                        }
                    } else {
                        initiateBluetooth();
                    }
                } catch (Exception ex) {
                    showToast(ex.getMessage());
                }
            }
        });

        btnUpdatePhone.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i("[BLUETOOTH]", "Attempting to send data");
                try {
                    if (mmSocket.isConnected() && btt != null) {
                        Editable phoneText = etPhone.getText();
                        Editable phoneAgencyText = etPhoneAgency.getText();
                        if (phoneText.length() > 0) {
                            String msgPhone = "103:" + phoneText.toString() + "==";
                            Log.d("[BLUETOOTH]", msgPhone);
                            btt.write(msgPhone.getBytes());
                        }
                        if (phoneAgencyText.length() > 0) {
                            String msgPhoneAgency = "104:" + phoneAgencyText.toString() + "==";
                            Log.d("[BLUETOOTH]", msgPhoneAgency);
                            btt.write(msgPhoneAgency.getBytes());
                        }
                    } else {
                        initiateBluetooth();
                    }
                } catch (Exception ex) {
                    showToast(ex.getMessage());
                }
            }
        });


        bta = BluetoothAdapter.getDefaultAdapter();
        //if bluetooth is not enabled then create Intent for user to turn it on
        if (!bta.isEnabled()) {
            Intent enableBTIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBTIntent, REQUEST_ENABLE_BT);
        } else {
            initiateBluetooth();
        }

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
                Toast.makeText(SettingActivity.this,
                        Text, Toast.LENGTH_LONG).show();
            }
        });
    }
}
