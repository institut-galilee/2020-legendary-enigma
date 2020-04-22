package com.legendaryenigma.firealarm;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

public class DashboardActivity extends AppCompatActivity {

    ImageView ivVisual, ivSetting, ivTest, ivLogout;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dashboard);

        ivVisual = (ImageView) findViewById(R.id.iv_visual);
        ivSetting = (ImageView) findViewById(R.id.iv_setting);
        ivTest = (ImageView) findViewById(R.id.iv_testing);
        ivLogout = (ImageView) findViewById(R.id.iv_logout);

        ivVisual.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(DashboardActivity.this,
                        VisualizationActivity.class);
                startActivity(i);
            }
        });

        ivSetting.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(DashboardActivity.this,
                        SettingActivity.class);
                startActivity(i);
            }
        });

        ivTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(DashboardActivity.this, TestingActivity.class);
                startActivity(i);
            }
        });

        ivLogout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(DashboardActivity.this, LoginActivity.class);
                startActivity(i);
                finish();
            }
        });
    }
}
