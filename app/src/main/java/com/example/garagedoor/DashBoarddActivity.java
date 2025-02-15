//package com.example.garagedoor;
//
//import android.os.Bundle;
//import android.util.Log;
//import android.webkit.WebSettings;
//import android.webkit.WebView;
//import android.webkit.WebViewClient;
//import android.widget.TextView;
//import androidx.appcompat.app.AppCompatActivity;
//import org.eclipse.paho.android.service.MqttAndroidClient;
//
//import org.eclipse.paho.client.mqtt3.IMqttActionListener;
//import org.eclipse.paho.client.mqtt3.IMqttDeliveryToken;
//import org.eclipse.paho.client.mqtt3.MqttCallback;
//import org.eclipse.paho.client.mqtt3.MqttClient;
//import org.eclipse.paho.client.mqtt3.MqttException;
//import org.eclipse.paho.client.mqtt3.MqttMessage;
//import org.json.JSONException;
//import org.json.JSONObject;
//
//public class DashBoarddActivity extends AppCompatActivity {
//    private WebView thingsBoardWebView;
//    private TextView mqttDataTextView;
//    private MqttAndroidClient mqttClient;
//    private static final String MQTT_BROKER_URL = "tcp://demo.thingsboard.io:1883"; // ThingsBoard broker
//    private static final String MQTT_TOPIC = "v1/devices/me/telemetry"; // Topic to receive telemetry data
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_dash_board);
//
//        // Set up WebView for ThingsBoard
//        thingsBoardWebView = findViewById(R.id.webView);
//        WebSettings webSettings = thingsBoardWebView.getSettings();
//        webSettings.setJavaScriptEnabled(true);
//        webSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
//        thingsBoardWebView.setWebViewClient(new WebViewClient() {
//            @Override
//            public boolean shouldOverrideUrlLoading(WebView view, String url) {
//                view.loadUrl(url);
//                return true;
//            }
//        });
//
//        // Replace with your ThingsBoard dashboard URL
//        String thingsBoardUrl = "https://demo.thingsboard.io/dashboard/be8e4b10-9de6-11ef-af67-a38a7671daf5?publicId=ac5818a0-85ee-11ef-a637-19503a4fe92c";
//        thingsBoardWebView.loadUrl(thingsBoardUrl);
//
//        // Set up TextView to display MQTT data
//        mqttDataTextView = findViewById(R.id.mqttDataTextView);
//
//        // Initialize MQTT client
//        mqttClient = new MqttAndroidClient(getApplicationContext(), MQTT_BROKER_URL, MqttClient.generateClientId());
//
//        try {
//            mqttClient.connect(null, new IMqttActionListener() {
//                @Override
//                public void onSuccess(IMqttToken asyncActionToken) {
//                    Log.d("MQTT", "Connected to ThingsBoard");
//                    subscribeToTopic();
//                }
//
//                @Override
//                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
//                    Log.e("MQTT", "Failed to connect to broker", exception);
//                }
//            });
//        } catch (MqttException e) {
//            e.printStackTrace();
//        }
//    }
//
//    private void subscribeToTopic() {
//        try {
//            mqttClient.subscribe(MQTT_TOPIC, 0, null, new IMqttActionListener() {
//                @Override
//                public void onSuccess(IMqttToken asyncActionToken) {
//                    Log.d("MQTT", "Subscribed to topic: " + MQTT_TOPIC);
//                }
//
//                @Override
//                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
//                    Log.e("MQTT", "Failed to subscribe to topic", exception);
//                }
//            });
//
//            mqttClient.setCallback(new MqttCallback() {
//                @Override
//                public void connectionLost(Throwable cause) {
//                    Log.e("MQTT", "Connection lost", cause);
//                }
//
//                @Override
//                public void messageArrived(String topic, MqttMessage message) {
//                    String payload = new String(message.getPayload());
//                    Log.d("MQTT", "Message received: " + payload);
//
//                    // Parse the JSON payload and update the UI
//                    try {
//                        JSONObject jsonObject = new JSONObject(payload);
//                        String doorStatus = jsonObject.getString("door_status");
//                        int doorDistance = jsonObject.getInt("door_distance");
//
//                        // Update the UI on the main thread
//                        runOnUiThread(() -> {
//                            mqttDataTextView.setText("Door Status: " + doorStatus + "\nDoor Distance: " + doorDistance + " cm");
//                        });
//                    } catch (JSONException e) {
//                        e.printStackTrace();
//                    }
//                }
//
//                @Override
//                public void deliveryComplete(IMqttDeliveryToken token) {}
//            });
//        } catch (MqttException e) {
//            e.printStackTrace();
//        }
//    }
//
//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        // Disconnect MQTT client on activity destruction
//        try {
//            if (mqttClient != null) {
//                mqttClient.disconnect();
//            }
//        } catch (MqttException e) {
//            e.printStackTrace();
//        }
//    }
//}
