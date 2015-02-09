package com.metabuilder.mobileapp;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.opengl.*;
//import javax.microedition.khronos.egl.EGLConfig;
//import javax.microedition.khronos.opengles.GL10;

public class mobileapp extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		m_assetManager = getResources().getAssets();
		
		m_glSurfaceView = new GLSurfaceView(this);
		m_glSurfaceView.setEGLContextClientVersion(2);
		m_glSurfaceView.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
		m_glSurfaceView.setRenderer(new GLESRenderer());
		setContentView(m_glSurfaceView);

		AndroidJNI.OnAppInit(m_assetManager);
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();

		AndroidJNI.OnAppShutdown();
	}
	
	@Override
	public void onPause()
	{
		super.onPause();
		m_glSurfaceView.onPause();

		AndroidJNI.OnAppPause();
	}
	
	@Override
	public void onResume()
	{
		super.onResume();
		m_glSurfaceView.onResume();
		
		AndroidJNI.OnAppResume();
	}

	//Load the native library
	static {
		System.loadLibrary("mobileapp3d");
	}
	
	private GLSurfaceView	m_glSurfaceView;
	private AssetManager	m_assetManager;
}

