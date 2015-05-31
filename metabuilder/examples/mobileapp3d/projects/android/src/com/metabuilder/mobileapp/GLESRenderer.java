package com.metabuilder.mobileapp;

import android.opengl.*;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLESRenderer implements GLSurfaceView.Renderer 
{
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) 
	{
		Log.d(TAG, "onSurfaceCreated");
		AndroidJNI.OnViewInit();
	}
	
	@Override
	public void onSurfaceChanged(GL10 gl, int w, int h)	 
	{
		Log.d(TAG, "onSurfaceChanged");
		AndroidJNI.OnSurfaceChanged(w, h);
	}

	@Override
	public void onDrawFrame(GL10 gl) 
	{
		AndroidJNI.OnDrawFrame();
 	}
	
	private static final String TAG = "GLESRenderer";	
}
