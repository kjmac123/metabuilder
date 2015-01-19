package com.metabuilder.mobileapp;

import android.app.Activity;
import android.os.Bundle;

public class mobileapp extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		System.out.println("Init");
		mobileappNativeInit();

		System.out.println("Running native code test function");
		mobileappNativeRun();
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();

		System.out.println("Shutdown");
		mobileappNativeShutdown();
	}

	//Native API
	public native void mobileappNativeInit();
	public native void mobileappNativeShutdown();
	public native void mobileappNativeRun();

	//Load the native library
	static {
		System.loadLibrary("mobileapp");
	}
}
