package com.metabuilder.mobileapp;

import android.opengl.*;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLESRenderer implements GLSurfaceView.Renderer {
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		AndroidJNI.OnViewInit();		
	}
	
	@Override
		public void onSurfaceChanged(GL10 gl, int w, int h)	 {
		AndroidJNI.OnSurfaceChanged(w, h);
	}

	@Override
	public void onDrawFrame(GL10 gl) {
		AndroidJNI.OnDrawFrame();
	}	
}
