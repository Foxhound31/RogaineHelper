package com.rogainer.rogainer;

import android.os.Bundle;
import android.support.v7.preference.PreferenceDialogFragmentCompat;

public class ViewSettingsPreferenceFragment extends PreferenceDialogFragmentCompat {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Load the preferences from an XML resource
        //addPreferencesFromResource(R.xml.view_preferences);
    }

    @Override
    public void onDialogClosed(boolean positiveResult) {

    }
}
