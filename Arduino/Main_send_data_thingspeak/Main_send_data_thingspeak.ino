#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "_______";        // Your Wi-Fi SSID
const char* password = "*********"; // Your Wi-Fi Password

const char* server = "http://api.thingspeak.com/update";
String apiKey = "***************";   // ThingSpeak API Key

void setup() {
  Serial.begin(115200);  // Communication with Arduino
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX (16), TX (17) from Arduino

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readString();
    Serial.print("Received data from Arduino: ");
    Serial.println(data);

    // Parse the received data
    int humidity, soundLevel;
    float temperature, heatIndex, mq2_ppm, mq3_ppm, mq135_ppm, dustDensity;
    sscanf(data.c_str(), "%d,%f,%f,%d,%f,%f,%f,%f", &humidity, &temperature, &heatIndex, &soundLevel, &mq2_ppm, &mq3_ppm, &mq135_ppm, &dustDensity);

    // Send data to ThingSpeak
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String(server) + "?api_key=" + apiKey +
                   "&field1=" + String(humidity) +
                   "&field2=" + String(temperature) +
                   "&field3=" + String(heatIndex) +
                   "&field4=" + String(soundLevel) +
                   "&field5=" + String(mq2_ppm) +
                   "&field6=" + String(mq3_ppm) +
                   "&field7=" + String(mq135_ppm) +
                   "&field8=" + String(dustDensity);
                   
     // Serial.println("URL: " + url);  // Debug the constructed URL
      http.begin(url);
      int httpCode = http.GET();

      if (httpCode > 0) {
        String response = http.getString();
       Serial.println("Data sent to ThingSpeak successfully");
       Serial.print(mq2_ppm);
       Serial.print(mq3_ppm );
       Serial.print(mq135_ppm);

        Serial.println("Response: " + response); // Print the server response
      } else {
       Serial.println("Failed to send data to ThingSpeak, HTTP error code: " + String(httpCode));
      }

      http.end();
    } else {
      Serial.println("Wi-Fi disconnected");
    }

    delay(15000);  // Wait 15 seconds before the next update
  }
}
