package com.cvhci.bvsa;
// File automatically created, do not change anything
public class NeededLibs {

	public static void loadLibs(){
		//load shared libraries
		System.loadLibrary("gnustl_shared");
		System.loadLibrary("BvsA");
		System.loadLibrary("ExampleCV");
		System.loadLibrary("AndroidCV");

	}

}
