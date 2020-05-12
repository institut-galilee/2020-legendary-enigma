package com.legendaryenigma.firealarm;

import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import okhttp3.FormBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class LoginActivity extends AppCompatActivity {

    EditText etDevice, etPassword;
    TextView tvRegister;
    Button btnLogin;

    final String url_Login = "http://172.31.95.97/login";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        etDevice = (EditText) findViewById(R.id.et_device);
        etPassword = (EditText) findViewById(R.id.et_password);
        btnLogin = (Button) findViewById(R.id.btn_login);
        tvRegister = (TextView) findViewById(R.id.tv_help);

        btnLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String deviceId = etDevice.getText().toString();
                String password = etPassword.getText().toString();

                new LoginUser().execute(deviceId, password);
            }
        });
    }




    public class LoginUser extends AsyncTask<String, Void, String>{

        @Override
        protected String doInBackground(String... strings) {
            String deviceId = strings[0];
            String password = strings[1];

            // Intent it = new Intent(LoginActivity.this,
            //         DashboardActivity.class);
            // startActivity(it);
            // finish();
            // return null;

           OkHttpClient okHttpClient = new OkHttpClient();
           RequestBody formBody = new FormBody.Builder()
                   .add("device_id", deviceId)
                   .add("password", password)
                   .build();

           Request request = new Request.Builder()
                   .url(url_Login)
                   .post(formBody)
                   .build();

           Response response = null;
           try{
               response = okHttpClient.newCall(request).execute();
               if(response.isSuccessful()){
                   String result = response.body().string();
                   String expected = "{\"message\":"+"\""+ deviceId +"\"}";
                   Log.d("debug_tag", result);
                   Log.d("debug_tag", expected);
                   if(result.equalsIgnoreCase(expected)){
                       Intent i = new Intent(LoginActivity.this,
                               DashboardActivity.class);
                       startActivity(i);
                       finish();
                   }else{
                      showToast("Device's Id or Password mismatched!");
                   }
               }
           }catch (Exception e){
               e.printStackTrace();
               showToast(e.getMessage());
           }
           return null;
        }
    }



    public void showToast(final String Text){
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(LoginActivity.this,
                        Text, Toast.LENGTH_LONG).show();
            }
        });
    }
}
