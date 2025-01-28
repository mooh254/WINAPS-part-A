#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SoftwareSerial.h>

// Define pins for SoftwareSerial communication
//const int RX_PIN = 0; // RX pin (to ESP32 TX)
//const int TX_PIN = 1; // TX pin (to ESP32 RX)

//SoftwareSerial espSerial(RX_PIN, TX_PIN); // RX, TX

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Dust Sensor settings
const int dustSensorPin = 4;
const int ledPowerPin = 9;
unsigned long samplingTime = 280;
unsigned long deltaTime = 40;
unsigned long sleepTime = 9680;
float dustDensity = 0.0;

// DHT22 Sensor settings
int DHTPin = 2;  
#define DHTTYPE DHT22
DHT dht(DHTPin, DHTTYPE);

float t, hic;
int h;

// Sound Sensor settings
int digitalPin_SS = 7;
int AnalogPin_SS = 0;
int BuzzerPin = 13;
int LedPin = 8;
int digitalVal;
int analogVal;
float dBValue;

// MQ2 Gas Sensor settings
#define MQ2_AnalogPin 1
const int MQ2_Threshold = 30;
float Analog_sensorVal_MQ2;
float MQ2Voltage_Val;
float ppm_MQ2;

// MQ3 Gas Sensor settings
float Analogpin_MQ3 = 2;
float SensorVal_Analog_MQ3;
float MQ3Voltage_Val;
float ppm_MQ3;
const int MQ3Threshold = 1000;

// MQ135 Gas Sensor
int MQ135_analogPin = 3;
float MQ135_Sensor_val;
float MQ135_SensVoltage;
float ppm_MQ135;
const int MQ135_Threshold = 1.0;

// Define scaling factors for each sensor
const float MQ2ScalingFactor = 0.0345; // Scaling factor for MQ2
const float MQ3ScalingFactor = 1; // Scaling factor for MQ3
const float MQ135ScalingFactor = 0.000417; // Scaling factor for MQ135
const float SS_ScalingFactor = 0.0509; // Scaling factor for SS

// Modified timing variables
unsigned long previousMillis = 0;
const unsigned long displayInterval = 3000;
int currentDisplay = 0;
bool readyToSendData = false;

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
  //espSerial.begin(9600);         
  lcd.begin();
  lcd.backlight();

  pinMode(digitalPin_SS, INPUT);
  pinMode(AnalogPin_SS, INPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(LedPin, OUTPUT);
  pinMode(ledPowerPin, OUTPUT);
  digitalWrite(ledPowerPin, LOW);

  dht.begin();
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  
    unsigned long currentMillis = millis();
    
    // Read all sensors
    updateSensorReadings();
    
    // Display readings based on timing
    if (currentMillis - previousMillis >= displayInterval) {
        previousMillis = currentMillis;
        
        switch(currentDisplay) {
            case 0:
               //Serial.print("case 0 =");
               ///Serial.println(currentDisplay);
                displayDHTReadings();
                break;
            case 1:
                //Serial.print("case 1 =");
                //Serial.println(currentDisplay);
                displaySoundSensorReadings();
                break;
            case 2:
                //Serial.print("case 2 =");
                //Serial.println(currentDisplay);
                displayMQ2Readings();
                break;
            case 3:
                //Serial.print("case 3 =");
                //Serial.println(currentDisplay);
                displayMQ3Readings();
                break;
            case 4:
                //Serial.print("case 4 =");
                //Serial.println(currentDisplay);
                displayMQ135Readings();
                break;
            case 5:
               // Serial.print("case 5 =");
                //Serial.println(currentDisplay);
                float dustDensity = readDustSensor();
                displayDustSensorReadings(dustDensity);
                // Set flag to send data after all sensors are read
                readyToSendData = true;
                break;
        }
        
//        // Send data only after dust sensor (last sensor) readings
        if (readyToSendData) {
            sendDataToESP32();
            readyToSendData = false;
        }
        
        currentDisplay = (currentDisplay + 1) % 6;  // Cycle through displays
    }

}
void updateSensorReadings() {
    // DHT Sensor
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
        hic = dht.computeHeatIndex(t, h, false);
    }

    // Sound Sensor
    digitalVal = digitalRead(digitalPin_SS);
    analogVal = analogRead(AnalogPin_SS);
    dBValue = analogVal * SS_ScalingFactor;

    // MQ2 Gas Sensor
    Analog_sensorVal_MQ2 = analogRead(MQ2_AnalogPin);
    ppm_MQ2 = Analog_sensorVal_MQ2 * MQ2ScalingFactor;

    // MQ3 Gas Sensor
    SensorVal_Analog_MQ3 = analogRead(Analogpin_MQ3);
    ppm_MQ3 = SensorVal_Analog_MQ3 * MQ3ScalingFactor;

    // MQ135 Gas Sensor
    MQ135_Sensor_val = analogRead(MQ135_analogPin);
    ppm_MQ135 = MQ135_Sensor_val * MQ135ScalingFactor;
}

// [Other display functions remain unchanged...]
// Display functions for each sensor are unchanged
void displayDHTReadings() {
 // Serial.println("     ");
//  Serial.println("_");
//  Serial.println("DHT Readings");
 // Serial.print(F("Humidity: "));
 // Serial.print(h);
  //Serial.print(F("%  Temp: "));
 // Serial.print(t);
 // Serial.print(F("°C  HeatIndex: "));
 // Serial.println(hic);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print((char)223);
  lcd.print("C");
}

void displaySoundSensorReadings() {
 // Serial.println("Sound Sensor");
  //Serial.print(F("Sound Levels: "));
 // Serial.println(analogVal);
  //Serial.print("  dB: ");
  //Serial.println(dBValue);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sound: ");
  lcd.print(dBValue);
  lcd.setCursor(0,1);
  lcd.print(" dB");
  
  if (digitalVal == HIGH) {
    digitalWrite(LedPin, HIGH);
   // Serial.println("High noise detected!");
    lcd.setCursor(0, 1);
    lcd.print("High noise!");
  } else {
    digitalWrite(LedPin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Moderate Levels");
  }
}
void displayMQ2Readings() {
 // Serial.println("MQ2 Gas Sensor");
//  Serial.print("Sensor Value: ");
 // Serial.print(Analog_sensorVal_MQ2);
 // Serial.print("  CO ppm: ");
 // Serial.println(ppm_MQ2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO: ");
  lcd.print(ppm_MQ2);
  lcd.print("ppm");

  if (ppm_MQ2 > MQ2_Threshold) {
    //Serial.println("| CO detected!");
    lcd.setCursor(0, 1);
    lcd.print("CO detected!");
    tone(BuzzerPin, 1000);
  } else {
    noTone(BuzzerPin);
    lcd.setCursor(0, 1);
    //lcd.print("");
  }
}

void displayMQ3Readings() {
 // Serial.println("MQ3 Gas Sensor");
  //Serial.print("Sensor Value: ");
  //Serial.print(SensorVal_Analog_MQ3);
  //Serial.print("  Methane ppm: ");
  //Serial.println(ppm_MQ3);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Methane:");
  lcd.print(ppm_MQ3);
  lcd.setCursor(0, 1);
  lcd.print("ppm");

  if (ppm_MQ3 > MQ3Threshold) {
   // Serial.println("High Methane levels detected!!");
    lcd.setCursor(0, 1);
    //lcd.print("High Alcohol!");
    tone(BuzzerPin, 1000);
  } else {
    //Serial.println("Average methane levels");
    lcd.setCursor(0, 1);
    //lcd.print("Avg Alcohol");
    noTone(BuzzerPin);
  }
}

void displayMQ135Readings() {
//  Serial.println("MQ135 Gas Sensor");
 // Serial.print("Sensor Value: ");
 // Serial.print(MQ135_Sensor_val);
 // Serial.print("  NO ppm: ");
  //Serial.println(ppm_MQ135);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NO:");
  lcd.print(ppm_MQ135);
  //lcd.setCursor(0, 1);
  lcd.print("ppm");

  if (ppm_MQ135 > MQ135_Threshold) {
    Serial.println("Unhealthy NO levels");
    lcd.setCursor(0, 1);
    lcd.print("Unhealthy");
    tone(BuzzerPin, 1000);
  } else {
    //Serial.println("Moderate levels");
    lcd.setCursor(0, 1);
    lcd.print("Moderate NO");
    noTone(BuzzerPin);
  }
}

float readDustSensor() {
  digitalWrite(ledPowerPin, HIGH);
  delayMicroseconds(samplingTime);

  int sensorValue = analogRead(dustSensorPin);
  digitalWrite(ledPowerPin, LOW);

  float voltage = sensorValue * (5.0 / 1024.0);
  dustDensity = (voltage - 0.6) * 0.5;
  
  return dustDensity > 0 ? dustDensity : 0;
}

void displayDustSensorReadings(float dustDensity) {
  //Serial.println("Dust Sensor");
  //Serial.print("Dust Density: ");
  //Serial.print(dustDensity);
  //Serial.println(" ug/m³");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dust:");
  lcd.print(dustDensity);
  lcd.setCursor(0, 1);
  lcd.print("ug/m3");
}

void sendDataToESP32() {
    // Format data string
    String data = String(h) + "," + 
                 String(t) + "," + 
                 String(hic) + "," + 
                 String(dBValue) + "," + 
                 String(ppm_MQ2) + "," + 
                 String(ppm_MQ3) + "," + 
                 String(ppm_MQ135) + "," + 
                 String(dustDensity);

    // Send data
    Serial.println(data);
    
    // Debug output
    //Serial.println("Data sent to ESP32: " + data);
}