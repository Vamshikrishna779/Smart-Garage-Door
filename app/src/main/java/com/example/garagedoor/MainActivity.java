package com.example.garagedoor;


import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {
    private TextView doorStatusText;
    private Button openButton, closeButton, dashboardButton, historyButton, settingsButton;
    private DatabaseReference firebaseDatabase;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        doorStatusText = findViewById(R.id.doorStatusText);
        openButton = findViewById(R.id.openButton);
        closeButton = findViewById(R.id.closeButton);
        dashboardButton = findViewById(R.id.dashboardButton);
        historyButton = findViewById(R.id.historyButton);
        settingsButton = findViewById(R.id.settingsButton);

        firebaseDatabase = FirebaseDatabase.getInstance().getReference();

        // Retrieve door status from Firebase
        firebaseDatabase.child("door_status").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot snapshot) {
                String status = snapshot.getValue(String.class);
                doorStatusText.setText("Door Status: " + status);
            }

            @Override
            public void onCancelled(DatabaseError error) {
                doorStatusText.setText("Error fetching status.");
            }
        });

        openButton.setOnClickListener(v -> firebaseDatabase.child("manual_control").setValue("open"));
        closeButton.setOnClickListener(v -> firebaseDatabase.child("manual_control").setValue("close"));

        // Navigate to Dashboard, History, and Settings Activities
        dashboardButton.setOnClickListener(v -> startActivity(new Intent(this, DashboardActivity.class)));
        historyButton.setOnClickListener(v -> startActivity(new Intent(this, HistoryActivity.class)));
        settingsButton.setOnClickListener(v -> startActivity(new Intent(this, SettingsActivity.class)));
    }
}

//
//import android.graphics.Color;
//import android.os.Bundle;
//import android.view.View;
//import android.widget.Button;
//import android.widget.TextView;
//import android.widget.Toast;
//import androidx.appcompat.app.AppCompatActivity;
//import com.google.firebase.database.DatabaseReference;
//import com.google.firebase.database.FirebaseDatabase;
//import com.google.firebase.database.DataSnapshot;
//import com.google.firebase.database.DatabaseError;
//import com.google.firebase.database.ValueEventListener;
//
//public class MainActivity extends AppCompatActivity {
//
//    private DatabaseReference firebaseDatabase;
//    private TextView txtDoorStatus;
//    private TextView txtDistance;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_main);
//
//        // Initialize Firebase
//        firebaseDatabase = FirebaseDatabase.getInstance().getReference();
//
//        // Initialize UI elements
//        Button openButton = findViewById(R.id.buttonOpen);
//        Button closeButton = findViewById(R.id.buttonClose);
//        txtDoorStatus = findViewById(R.id.txtDoorStatus);
//        txtDistance = findViewById(R.id.txtDistance);
//
//        // Open door button click listener
//        openButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                // Change button color to indicate action
//                openButton.setBackgroundColor(Color.GREEN); // Set to desired color
//                sendCommandToFirebase("open");
//
//                // Restore original color after a delay
//                openButton.postDelayed(() -> openButton.setBackgroundColor(Color.TRANSPARENT), 1000);
//            }
//        });
//
//        // Close door button click listener
//        closeButton.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                // Change button color to indicate action
//                closeButton.setBackgroundColor(Color.GREEN); // Set to desired color
//                sendCommandToFirebase("close");
//
//                // Restore original color after a delay
//                closeButton.postDelayed(() -> closeButton.setBackgroundColor(Color.TRANSPARENT), 1000);
//            }
//        });
//
//        // Fetch door status and distance on app start
//        fetchDoorStatus();
//        fetchDistanceData();
//    }
//
//    // Function to send a command to Firebase
//    private void sendCommandToFirebase(String command) {
//        firebaseDatabase.child("manual_control").setValue(command);
//    }
//
//    // Function to fetch door status from Firebase
//    private void fetchDoorStatus() {
//        firebaseDatabase.child("door_status").addValueEventListener(new ValueEventListener() {
//            @Override
//            public void onDataChange(DataSnapshot snapshot) {
//                String status = snapshot.getValue(String.class);
//                txtDoorStatus.setText("Door Status: " + status);
//            }
//
//            @Override
//            public void onCancelled(DatabaseError error) {
//                Toast.makeText(MainActivity.this, "Failed to fetch door status", Toast.LENGTH_SHORT).show();
//            }
//        });
//    }
//
//    // Function to fetch distance data from Firebase
//    private void fetchDistanceData() {
//        firebaseDatabase.child("door_distance").addValueEventListener(new ValueEventListener() {
//            @Override
//            public void onDataChange(DataSnapshot snapshot) {
//                Integer distance = snapshot.getValue(Integer.class);
//                if (distance != null) {
//                    txtDistance.setText("Distance: " + distance + " cm");
//                } else {
//                    txtDistance.setText("Distance data unavailable");
//                }
//            }
//
//            @Override
//            public void onCancelled(DatabaseError error) {
//                Toast.makeText(MainActivity.this, "Failed to fetch distance", Toast.LENGTH_SHORT).show();
//            }
//        });
//    }
//}
