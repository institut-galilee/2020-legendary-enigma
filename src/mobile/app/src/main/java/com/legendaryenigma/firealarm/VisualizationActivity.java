package com.legendaryenigma.firealarm;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import androidx.appcompat.app.AppCompatActivity;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;
import com.github.mikephil.charting.utils.ColorTemplate;

public class VisualizationActivity extends AppCompatActivity {

    private RelativeLayout mainLayout;
    private LineChart mChart;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_visualization);
        mainLayout = (RelativeLayout) findViewById(R.id.main_layout);
        mChart = (LineChart) findViewById(R.id.chart);
        mChart.setMinimumHeight(1200);
        Description description = new Description();
        description.setText("Temperature graph in realtime");
        mChart.setDescription(description);
        mChart.setNoDataText("No data for the moment");

        mChart.setHighlightPerDragEnabled(true);
        mChart.setTouchEnabled(true);
        mChart.setDragEnabled(true);
        mChart.setScaleEnabled(true);
        mChart.setDrawGridBackground(false);

       mChart.setPinchZoom(true);
       mChart.setBackgroundColor(Color.LTGRAY);

       LineData data = new LineData();
       data.setValueTextColor(Color.WHITE);

       mChart.setData(data);

        Legend l = mChart.getLegend();

        l.setForm(Legend.LegendForm.LINE);
        l.setTextColor(Color.WHITE);

        XAxis x1 = mChart.getXAxis();
        x1.setTextColor(Color.WHITE);
        x1.setDrawGridLines(false);
        x1.setAvoidFirstLastClipping(true);

        //temperature y-axis
        YAxis y1 = mChart.getAxisLeft();
        y1.setTextColor(Color.WHITE);
        y1.setAxisMaximum(60f);
        y1.setDrawGridLines(true);

        //gas y-axis
        YAxis y2 = mChart.getAxisRight();
        y2.setEnabled(false);
    }

    @Override
    protected void onResume() {
        super.onResume();

        new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i = 0; i < 100; i++) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            addEntry();
                        }
                    });

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException ex) {
                        ex.printStackTrace();
                    }
                }
            }
        }).start();
    }

    private void addEntry() {
        LineData data = mChart.getLineData();

        if (data != null) {
            ILineDataSet set = data.getDataSetByIndex(0);
            if (set == null) {
                set = createSet();
                data.addDataSet(set);
            }

            Entry e = new Entry();
            e.setY((float) Math.random() * 5 + 20);
            e.setX(set.getEntryCount());
            data.addEntry(e, 0);
            //
            mChart.notifyDataSetChanged();
            mChart.setVisibleXRange(4,6);
            mChart.moveViewToX(data.getEntryCount() - 6);
        }
    }

    private ILineDataSet createSet() {
        LineDataSet set = new LineDataSet(null, "Temperature");
        set.setCubicIntensity(0.2f);
        set.setAxisDependency(YAxis.AxisDependency.LEFT);
        set.setColor(ColorTemplate.getHoloBlue());
        set.setCircleColor(ColorTemplate.getHoloBlue());
        set.setLineWidth(3f);
        set.setCircleSize(5f);
        set.setFillAlpha(65);
        set.setFillColor(ColorTemplate.getHoloBlue());
        set.setHighLightColor(Color.YELLOW);
        set.setValueTextColor(Color.WHITE);
        set.setValueTextSize(10f);

        return set;




    }
}
