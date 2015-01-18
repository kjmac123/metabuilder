package com.metabuilder.mobileapp;

import android.app.Activity;
import android.os.Bundle;

public class MobileApp extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		System.out.println("Init");
		MobileAppNative_Init();

		System.out.println("Running native code test function");
		MobileAppNative_Run();
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();

		System.out.println("Shutdown");
		MobileAppNative_Shutdown();
	}

	//Native API
	public native void MobileAppNative_Init();
	public native void MobileAppNative_Shutdown();
	public native void MobileAppNative_Run();

	//Load the native library
	static {
		System.loadLibrary("mobileapp");
	}
}
