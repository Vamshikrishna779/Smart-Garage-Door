package com.example.garagedoor;

import android.os.Bundle;
import android.text.TextUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import androidx.appcompat.app.AppCompatActivity;

public class SettingsActivity extends AppCompatActivity {

    private Button buttonSaveWiFi;
    private EditText editTextSSID, editTextPassword;
    private Switch switchDoorLock;
    private TextView tvDoorLockStatus;
    private DatabaseReference doorLockRef, wifiRef;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        // Initialize views
        switchDoorLock = findViewById(R.id.switchDoorLock);
        tvDoorLockStatus = findViewById(R.id.tvDoorLockStatus);
        buttonSaveWiFi = findViewById(R.id.buttonSaveWiFi);
        editTextSSID = findViewById(R.id.ssidInput);
        editTextPassword = findViewById(R.id.passwordInput);


        // Initialize Firebase references
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        doorLockRef = database.getReference("door_lock");
        wifiRef = database.getReference("wifi");


        // Listen for changes in door lock status from Firebase
        doorLockRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot snapshot) {
                boolean isLocked = snapshot.getValue(Boolean.class);
                switchDoorLock.setChecked(isLocked);
                tvDoorLockStatus.setText(isLocked ? "Door is Locked" : "Door is Unlocked");
            }

            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(SettingsActivity.this, "Failed to load door lock status", Toast.LENGTH_SHORT).show();
            }
        });

        // Handle switch toggle to lock/unlock the door
        switchDoorLock.setOnCheckedChangeListener((buttonView, isChecked) -> {
            doorLockRef.setValue(isChecked).addOnCompleteListener(task -> {
                if (task.isSuccessful()) {
                    tvDoorLockStatus.setText(isChecked ? "Door is Locked" : "Door is Unlocked");
                    Toast.makeText(SettingsActivity.this, "Door " + (isChecked ? "locked" : "unlocked") + " successfully", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(SettingsActivity.this, "Failed to update lock status", Toast.LENGTH_SHORT).show();
                }
            });
        });



        // Wi-Fi Save button functionality
        buttonSaveWiFi.setOnClickListener(view -> saveWiFiDetails());
    }

    private void saveWiFiDetails() {
        // Get SSID and Password from input fields
        String newSSID = editTextSSID.getText().toString().trim();
        String newPassword = editTextPassword.getText().toString().trim();

        // Validate inputs
        if (TextUtils.isEmpty(newSSID) || TextUtils.isEmpty(newPassword)) {
            Toast.makeText(this, "SSID and Password cannot be empty", Toast.LENGTH_SHORT).show();
            return;
        }

        // Update Wi-Fi details in Firebase
        wifiRef.child("ssid").setValue(newSSID);
        wifiRef.child("password").setValue(newPassword).addOnCompleteListener(task -> {
            if (task.isSuccessful()) {
                Toast.makeText(SettingsActivity.this, "Wi-Fi details updated successfully", Toast.LENGTH_SHORT).show();
                editTextSSID.setText("");
                editTextPassword.setText(""); // Clear the fields after saving
            } else {
                Toast.makeText(SettingsActivity.this, "Failed to update Wi-Fi details", Toast.LENGTH_SHORT).show();
            }
        });
    }
}


//
//import androidx.appcompat.app.AppCompatActivity;
//import android.os.Bundle;
//import android.text.TextUtils;
//import android.widget.Button;
//import android.widget.EditText;
//import android.widget.Switch;
//import android.widget.TextView;
//import android.widget.Toast;
//
//import com.google.firebase.database.DataSnapshot;
//import com.google.firebase.database.DatabaseError;
//import com.google.firebase.database.DatabaseReference;
//import com.google.firebase.database.FirebaseDatabase;
//import com.google.firebase.database.ValueEventListener;
//
//public class SettingsActivity extends AppCompatActivity {
//
//    private Button buttonDoorControl, buttonSaveWiFi;
//    private EditText editTextSSID, editTextPassword;
//    private Switch switchDoorLock;
//    private TextView tvDoorLockStatus;
//    private DatabaseReference doorLockRef, wifiRef;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_settings);
//
//        // Initialize views
//        switchDoorLock = findViewById(R.id.switchDoorLock);
//        tvDoorLockStatus = findViewById(R.id.tvDoorLockStatus);
//        buttonSaveWiFi = findViewById(R.id.buttonSaveWiFi);
//        editTextSSID = findViewById(R.id.ssidInput);
//        editTextPassword = findViewById(R.id.passwordInput);
//        buttonDoorControl = findViewById(R.id.buttonDoorControl);
//
//        // Initialize Firebase references
//        FirebaseDatabase database = FirebaseDatabase.getInstance();
//        doorLockRef = database.getReference("door_lock");
//        wifiRef = database.getReference("wifi");
//
//        // Listen for changes in door lock status from Firebase
//        doorLockRef.addValueEventListener(new ValueEventListener() {
//            @Override
//            public void onDataChange(DataSnapshot snapshot) {
//                boolean isLocked = snapshot.getValue(Boolean.class);
//                switchDoorLock.setChecked(isLocked);
//                tvDoorLockStatus.setText(isLocked ? "Door is Locked" : "Door is Unlocked");
//            }
//
//            @Override
//            public void onCancelled(DatabaseError error) {
//                Toast.makeText(SettingsActivity.this, "Failed to load door lock status", Toast.LENGTH_SHORT).show();
//            }
//        });
//
//        // Handle switch toggle to lock/unlock the door
//        switchDoorLock.setOnCheckedChangeListener((buttonView, isChecked) -> {
//            doorLockRef.setValue(isChecked).addOnCompleteListener(task -> {
//                if (task.isSuccessful()) {
//                    tvDoorLockStatus.setText(isChecked ? "Door is Locked" : "Door is Unlocked");
//                    Toast.makeText(SettingsActivity.this, "Door " + (isChecked ? "locked" : "unlocked") + " successfully", Toast.LENGTH_SHORT).show();
//                } else {
//                    Toast.makeText(SettingsActivity.this, "Failed to update lock status", Toast.LENGTH_SHORT).show();
//                }
//            });
//        });
//
//        // Door Control button functionality (handle door open/close)
//        buttonDoorControl.setOnClickListener(view -> toggleDoor());
//
//        // Wi-Fi Save button functionality
//        buttonSaveWiFi.setOnClickListener(view -> saveWiFiDetails());
//    }
//
//    private void saveWiFiDetails() {
//        // Get SSID and Password from input fields
//        String newSSID = editTextSSID.getText().toString().trim();
//        String newPassword = editTextPassword.getText().toString().trim();
//
//        // Validate inputs
//        if (TextUtils.isEmpty(newSSID) || TextUtils.isEmpty(newPassword)) {
//            Toast.makeText(this, "SSID and Password cannot be empty", Toast.LENGTH_SHORT).show();
//            return;
//        }
//
//        // Update Wi-Fi details in Firebase
//        wifiRef.child("ssid").setValue(newSSID);
//        wifiRef.child("password").setValue(newPassword).addOnCompleteListener(task -> {
//            if (task.isSuccessful()) {
//                Toast.makeText(SettingsActivity.this, "Wi-Fi details updated successfully", Toast.LENGTH_SHORT).show();
//                editTextSSID.setText("");
//                editTextPassword.setText(""); // Clear the fields after saving
//            } else {
//                Toast.makeText(SettingsActivity.this, "Failed to update Wi-Fi details", Toast.LENGTH_SHORT).show();
//            }
//        });
//    }
//
//    private void toggleDoor() {
//        // Code to toggle door open/close
//        // Depending on the door state, open or close the garage door
//        // Send control signals to Firebase or ESP32 to control the garage door
//    }
//}
