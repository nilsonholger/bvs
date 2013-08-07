package com.cvhci.bvsa;

import java.lang.reflect.Field;

import java.io.IOException;

import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;

import android.util.Log;

import android.app.ListActivity;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class MainActivity extends ListActivity {
	
	private static final String TAG = "MainActivity";
	
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		String[] elements = null;
		try{
			elements = getBaseContext().getResources().getAssets().list("conf");
		}
		catch (IOException e) {
			e.printStackTrace();
			Log.e(TAG,"Failed to load config. Exception thrown: " + e);
		}
		if(elements.length == 1) {
			//only one config, so override back button
			changeIntent(elements[0],true);
		}
		else {
			ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_list_item_1,
				elements);
                  //Set the elements using List Adapter
			this.setListAdapter(arrayAdapter);
		}
	}



	@Override

	protected void onListItemClick(ListView l, View v, int position, long id) {

		super.onListItemClick(l, v, position, id);
		Object o = this.getListAdapter().getItem(position);
		String element = o.toString();

		// if you can select an item, the list has more than 1 entry
		changeIntent(element,false);

	}

	private void changeIntent(String configFile, boolean oneConfig) {

		Log.i("changeIntent with CONFIGFILE", configFile);

		Intent viewIntent = new Intent(getBaseContext(), ViewActivity.class);
		viewIntent.putExtra("configFile", configFile);
		viewIntent.putExtra("oneConfig", oneConfig);
		startActivity(viewIntent);
	}





	
}