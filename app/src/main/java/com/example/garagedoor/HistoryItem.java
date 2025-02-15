package com.example.garagedoor;
public class HistoryItem {
    private String status;
    private String timestamp;

    public HistoryItem() {
        // Default constructor required for Firebase
    }

    public HistoryItem(String status, String timestamp) {
        this.status = status;
        this.timestamp = timestamp;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }
}
