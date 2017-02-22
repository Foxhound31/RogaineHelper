package com.rogainer.rogainer;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.content.SharedPreferences;
import android.util.Log;

public class ViewSettingsDialogFragment extends DialogFragment {
    /* The activity that creates an instance of this dialog fragment must
    * implement this interface in order to receive event callbacks.
    * Each method passes the DialogFragment in case the host needs to query it. */
    public interface ViewSettingsDialogListener {
        public void onDialogPositiveClick(DialogFragment dialog);
        public void onDialogNegativeClick(DialogFragment dialog);
        public void onDialogLabelsCheckedChanged(DialogFragment dialog, boolean b);
        public void onDialogUnderlayerItemSelected(DialogFragment dialog, int position, long id);
        public void onDialogOpacityChanged(DialogFragment dialog, int progress);
        public void onDialogNumbersCheckedChanged(DialogFragment dialog, boolean b);
        public void onDialogRoutesCheckedChanged(DialogFragment dialog, boolean b);
    }

    // Use this instance of the interface to deliver action events
    ViewSettingsDialogListener mListener;
    View mView;


    // Override the Fragment.onAttach() method to instantiate the NoticeDialogListener
    @Override
    public void onAttach(Context context) {
        super.onAttach(context);

         // Verify that the host activity implements the callback interface
        try {
            // Instantiate the NoticeDialogListener so we can send events to the host
            mListener = (ViewSettingsDialogListener) context;
        } catch (ClassCastException e) {
            // The activity doesn't implement the interface, throw exception
            throw new ClassCastException(context.toString()
                    + " must implement ViewSettingsDialogListener");
        }
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        // Use the Builder class for convenient dialog construction
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        // Get the layout inflater
        LayoutInflater inflater = getActivity().getLayoutInflater();
        mView = inflater.inflate(R.layout.view_dialog, null);

        // Inflate and set the layout for the dialog
        // Pass null as the parent view because its going in the dialog layout
        builder.setTitle(R.string.view_settings_dialog_title)
                .setView(mView)
                .setPositiveButton(R.string.view_settings_dialog_positive, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {

                        // Save preferences
//                        Log.d("myTag", "Button OK pressed");
//                        SharedPreferences.Editor editor = getActivity().getPreferences(Context.MODE_PRIVATE).edit();
//                        editor.putInt(getString(R.string.view_settings_file_key), newHighScore);
//                        editor.commit();

                        mListener.onDialogPositiveClick(ViewSettingsDialogFragment.this);
                    }
                })
                .setNegativeButton(R.string.view_settings_dialog_negative, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        Log.d("myTag", "Button CANCEL pressed");
                        mListener.onDialogNegativeClick(ViewSettingsDialogFragment.this);
                    }
                });
        // Create the AlertDialog object and return it
        return builder.create();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = mView;

        CheckBox chk1 = (CheckBox)view.findViewById(R.id.labelsCheckBox);
        chk1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mListener.onDialogLabelsCheckedChanged(ViewSettingsDialogFragment.this, b);
            }
        });

        Spinner sp = (Spinner)view.findViewById(R.id.underlayerSpinner);
        sp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                mListener.onDialogUnderlayerItemSelected(ViewSettingsDialogFragment.this, position, id);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        SeekBar sb = (SeekBar)view.findViewById(R.id.opacitySeekBar);
        sb.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener () {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                mListener.onDialogOpacityChanged(ViewSettingsDialogFragment.this, progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}

        });

        CheckBox chk2 = (CheckBox)view.findViewById(R.id.numbersCheckBox);
        chk2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mListener.onDialogNumbersCheckedChanged(ViewSettingsDialogFragment.this, b);
            }
        });

        CheckBox chk3 = (CheckBox)view.findViewById(R.id.routesCheckBox);
        chk3.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                mListener.onDialogRoutesCheckedChanged(ViewSettingsDialogFragment.this, b);
            }
        });

        return view;
    }
}
