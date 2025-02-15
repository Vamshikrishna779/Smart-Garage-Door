package com.example.garagedoor.Models;

public class UsageLog {
    private String action;
    private long timestamp;

    public UsageLog() {}

    public UsageLog(String action, long timestamp) {
        this.action = action;
        this.timestamp = timestamp;
    }

    // Getters and setters
}