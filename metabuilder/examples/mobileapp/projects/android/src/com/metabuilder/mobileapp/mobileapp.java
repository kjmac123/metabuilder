package com.metabuilder.mobileapp;

import android.os.Handler;
import android.util.Log;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import com.metabuilder.mobileapp.R.*;

public class mobileapp extends Activity
{
	private static final String TAG = "mobileapp";
	
    private Handler mUpdateHandler = new Handler();
    private Runnable mUpdateRunnable = new Runnable() 
    {
        public void run() {
            mUpdateHandler.postDelayed(this, 100);
            TextView mainLabel = (TextView)findViewById(id.MainLabel);
    		double test = mobileappNativeRun();
    		mainLabel.setText(String.format("%.1f seconds", test));
        }
    };
    
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		System.out.println("Init");
		mobileappNativeInit();
		
        setContentView(R.layout.activity_main);
        mUpdateHandler.postDelayed(mUpdateRunnable, 100);
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();

		System.out.println("Shutdown");
		mobileappNativeShutdown();
	}

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }	
	
	//Native API
	public native void mobileappNativeInit();
	public native void mobileappNativeShutdown();
	public native double mobileappNativeRun();

	//Load the native library
	static {
		System.loadLibrary("mobileapp");
		Log.i(TAG,"Native code loaded");
	}
}
