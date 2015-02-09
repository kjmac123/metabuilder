package com.metabuilder.mobileapp;

import android.content.res.AssetManager;

public class AndroidJNI
{
	//Native API
	public static native void OnAppInit(AssetManager mgr);
	public static native void OnAppShutdown();
	public static native void OnAppPause();
	public static native void OnAppResume();
	
	public static native void OnViewInit();

	public static native void OnSurfaceChanged(int w, int h);

	public static native void OnDrawFrame();
}

