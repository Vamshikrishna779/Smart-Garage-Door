package com.example.garagedoor;
//import android.os.Bundle;
//import android.webkit.WebResourceRequest;
//import android.webkit.WebView;
//import android.webkit.WebViewClient;
//import androidx.appcompat.app.AppCompatActivity;
//
//public class DashboardActivity extends AppCompatActivity {
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_dashboard);
//
//        // Initialize the WebView
//        WebView webView = findViewById(R.id.webView);
//
//        // Enable JavaScript if needed
//        webView.getSettings().setJavaScriptEnabled(true);
//
//        // Set WebViewClient to open links within the WebView instead of an external browser
//        webView.setWebViewClient(new WebViewClient());
//
//        // Load the URL
//        webView.loadUrl("https://www.amazon.in/");  // Replace with your URL
//    }
//
//    // Handle the back button to allow navigation within WebView
//    @Override
//    public void onBackPressed() {
//        WebView webView = findViewById(R.id.webView);
//        if (webView.canGoBack()) {
//            webView.goBack();  // Go back in the WebView history
//        } else {
//            super.onBackPressed();  // Default back behavior (exit the activity)
//        }
//    }
//}


import android.os.Bundle;
import android.webkit.CookieManager;
import android.os.Bundle;
import android.webkit.CookieManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import androidx.appcompat.app.AppCompatActivity;

public class DashboardActivity extends AppCompatActivity {
    private WebView thingsBoardWebView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dashboard);

        // Initialize the WebView
        thingsBoardWebView = findViewById(R.id.webView);

        // Enable JavaScript for ThingsBoard widgets
        WebSettings webSettings = thingsBoardWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);  // Enable JavaScript for interactivity
        webSettings.setDomStorageEnabled(true);  // Enable DOM storage for persistent state
        webSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);  // Allow mixed content (HTTP in HTTPS)

        // Enable cookies (important for login and session management)
        CookieManager cookieManager = CookieManager.getInstance();
        cookieManager.setAcceptCookie(true);  // Accept cookies for ThingsBoard authentication

        // Ensure links open within the WebView, not the external browser
        thingsBoardWebView.setWebViewClient(new WebViewClient() {
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url);  // Keep links inside the WebView
                return true;
            }
        });

        // Load the ThingsBoard dashboard URL (Replace with your actual dashboard URL)
        String thingsBoardUrl = "https://demo.thingsboard.io/dashboard/be8e4b10-9de6-11ef-af67-a38a7671daf5?publicId=ac5818a0-85ee-11ef-a637-19503a4fe92c";
        thingsBoardWebView.loadUrl(thingsBoardUrl);
    }

    // Handle the back button to allow navigation within the WebView
    @Override
    public void onBackPressed() {
        if (thingsBoardWebView.canGoBack()) {
            thingsBoardWebView.goBack();  // Go back to the previous page in WebView
        } else {
            super.onBackPressed();  // Exit the activity if there's no history
        }
    }
}
//
//import android.os.Bundle;
//import android.webkit.WebSettings;
//import android.webkit.WebView;
//import android.webkit.WebViewClient;
//import androidx.appcompat.app.AppCompatActivity;
//import org.eclipse.paho.client.mqttv3.MqttClient;
//import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
//import org.eclipse.paho.client.mqttv3.MqttMessage;
//import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
//
//public class DashboardActivity extends AppCompatActivity {
//    private WebView thingsBoardWebView;
//    private MqttClient mqttClient;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_dashboard);
//
//        thingsBoardWebView = findViewById(R.id.webView);
//
//        // Enable JavaScript for ThingsBoard widgets
//        WebSettings webSettings = thingsBoardWebView.getSettings();
//        webSettings.setJavaScriptEnabled(true);
//
//        // Set WebViewClient to handle navigation within the WebView
//        webSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
//        thingsBoardWebView.setWebViewClient(new WebViewClient() {
//            @Override
//            public boolean shouldOverrideUrlLoading(WebView view, String url) {
//                view.loadUrl(url);
//                return true;
//            }
//        });
//
//        // Load the ThingsBoard dashboard URL
//        String thingsBoardUrl = "https://demo.thingsboard.io/dashboard/be8e4b10-9de6-11ef-af67-a38a7671daf5?publicId=ac5818a0-85ee-11ef-a637-19503a4fe92c"; // Replace with your dashboard's public URL
//        thingsBoardWebView.loadUrl(thingsBoardUrl);
//
//        // Initialize MQTT client for real-time updates
//        initializeMQTTClient();
//    }
//
//    private void initializeMQTTClient() {
//        String brokerUrl = "tcp://demo.thingsboard.io:1883"; // Replace with your MQTT broker URL
//        String clientId = "dtx7kky8bwo3910ji6gr"; // Unique client ID
//        String username = "unused"; // Replace with your ThingsBoard MQTT username
//        String password = "unused"; // Replace with your ThingsBoard MQTT password
//
//        try {
//            mqttClient = new MqttClient(brokerUrl, clientId, null);
//
//            // Set connection options
//            MqttConnectOptions options = new MqttConnectOptions();
//            options.setUserName(username);
//            options.setPassword(password.toCharArray());
//            options.setCleanSession(true);
//
//            // Connect to the broker
//            mqttClient.connect(options);
//
//            // Subscribe to the topic for real-time updates (Replace with your topic)
//            String topic = "v1/devices/me/telemetry"; // Replace with the relevant topic
//            mqttClient.subscribe(topic, 1, new IMqttMessageListener() {
//                @Override
//                public void messageArrived(String topic, MqttMessage message) throws Exception {
//                    // Process the real-time message here
//                    String messageContent = new String(message.getPayload());
//
//                    // Update the WebView dynamically (if needed)
//                    // For example, you can update the WebView with new data based on the MQTT message
//                    thingsBoardWebView.post(new Runnable() {
//                        @Override
//                        public void run() {
//                            thingsBoardWebView.loadUrl("javascript:updateDashboard('" + messageContent + "')");
//                        }
//                    });
//                }
//            });
//
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//    }
//
//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        // Disconnect MQTT client when the activity is destroyed
//        if (mqttClient != null && mqttClient.isConnected()) {
//            try {
//                mqttClient.disconnect();
//            } catch (Exception e) {
//                e.printStackTrace();
//            }
//        }
//    }
//}
//
//////package com.example.garagedoorapp;

//
////
////import androidx.appcompat.app.AppCompatActivity;
////import android.os.Bundle;
////import android.widget.TextView;
////
////
////public class DashboardActivity extends AppCompatActivity {
////
////    private TextView txtWeeklySummary;
////
////
////    @Override
////    protected void onCreate(Bundle savedInstanceState) {
////        super.onCreate(savedInstanceState);
////        setContentView(R.layout.activity_dashboard);
////
////        txtWeeklySummary = findViewById(R.id.txtWeeklySummary);
////        lineChart = findViewById(R.id.lineChart);
////
////        displayUsageData();
////    }
////
////    private void displayUsageData() {
////        // Fetch usage data from Firebase and populate the chart (this is a simplified example)
////        // Assume dataPoints are fetched and added to the chart
////        LineDataSet dataSet = new LineDataSet(dataPoints, "Weekly Usage");
////        LineData lineData = new LineData(dataSet);
////        lineChart.setData(lineData);
////        lineChart.invalidate();  // Refresh the chart
////
////        txtWeeklySummary.setText("Weekly door usage data (open/close events)");
////    }
////}
