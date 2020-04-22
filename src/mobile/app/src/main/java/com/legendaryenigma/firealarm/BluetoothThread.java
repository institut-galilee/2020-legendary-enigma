package com.legendaryenigma.firealarm;

import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

class BluetoothThread extends Thread {

    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    public static final int RESPONSE_MESSAGE = 10;
    Handler uih;

    public BluetoothThread(BluetoothSocket socket, Handler uih){
        mmSocket = socket;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;
        this.uih = uih;
        Log.i("[THREAD-CT]","Creating thread");
        try{
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();

        } catch(IOException e) {
            Log.e("[THREAD-CT]","Error:"+ e.getMessage());
        }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;

        try {
            mmOutStream.flush();
        } catch (IOException e) {
            return;
        }
        Log.i("[THREAD-CT]","IO's obtained");

    }

    public void run(){
        //byte[] buffer = new byte[1024];
        //int bytes;
        BufferedReader br = new BufferedReader(new InputStreamReader(mmInStream));
        Log.i("[THREAD-CT]","Starting thread");
        while(true){
            try{
                // bytes = mmInStream.read(buffer);
                String resp = br.readLine();
                //Transfer these data to the UI thread
                Message msg = new Message();
                msg.what = RESPONSE_MESSAGE;
                msg.obj = resp;
                uih.sendMessage(msg);
            }catch(IOException e){
                break;
            }
        }
        Log.i("[THREAD-CT]","While loop ended");
    }

    public String write(byte[] bytes){
        try{
            Log.i("[THREAD-CT]", "Writting bytes");
            mmOutStream.write(bytes);
            mmOutStream.flush();
            Thread.sleep(10);
            return null;
        } catch (IOException | InterruptedException e){
            Log.d("[BLUETOOTH]", e.getMessage());
            return e.getMessage();
        }
    }

    public void cancel(){
        try{
            mmSocket.close();
        }catch(IOException e){}
    }
}
