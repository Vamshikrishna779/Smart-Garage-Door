package com.example.garagedoor;

import android.util.Log;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

public class FirebaseService {
    private DatabaseReference dbRef;

    public FirebaseService() {
        dbRef = FirebaseDatabase.getInstance().getReference();
    }

    public void logUsage(String action) {
        String timestamp = String.valueOf(System.currentTimeMillis());
        dbRef.child("usage_logs").child(timestamp).setValue(action);
    }
}
