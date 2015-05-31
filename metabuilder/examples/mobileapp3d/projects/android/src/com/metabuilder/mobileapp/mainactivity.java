package com.metabuilder.mobileapp;

import android.app.Activity;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Point;
import android.os.Bundle;
import android.opengl.*;
import android.util.Log;
import android.view.Display;


public class mainactivity extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		Log.d(TAG, "onCreate");
		
		m_assetManager = getResources().getAssets();
		AndroidJNI.OnAppInit(m_assetManager);
	
		m_glSurfaceView = new GLSurfaceView(this);
		m_glSurfaceView.setEGLContextClientVersion(2);
		m_glSurfaceView.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
		m_glSurfaceView.setRenderer(new GLESRenderer());
		setContentView(m_glSurfaceView);
	}

	@Override
	public void onDestroy()
	{
		Log.d(TAG, "onDestroy");
		super.onDestroy();
	}
	
	@Override
	public void onPause()
	{
		super.onPause();
		Log.d(TAG, "onPause");
		
		m_glSurfaceView.onPause();

		AndroidJNI.OnAppPause();
	}
	
	@Override
	public void onResume()
	{
		super.onResume();
		Log.d(TAG, "onResume");
		
		m_glSurfaceView.onResume();
		
		AndroidJNI.OnAppResume();
	}
	
	@Override
	public void onStop()
	{
		super.onStop();
		Log.d(TAG, "onStop");
		
		AndroidJNI.OnAppShutdown();
	}

	//Load the native library
	static {
		System.loadLibrary("mobileapp3d");
	}	
	  
	private static final String TAG = "mainactivity";
	private AssetManager	m_assetManager;	  
	private GLSurfaceView	m_glSurfaceView;
}

