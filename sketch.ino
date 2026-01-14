// ESP32 Smart Classroom Recorder with AI
// Wokwi + Blynk IoT

#define BLYNK_TEMPLATE_ID "TMPL5g37-EHgF"
#define BLYNK_TEMPLATE_NAME "AIoT Task"
#define BLYNK_AUTH_TOKEN "RHXPNIObqDlGrwk1tAtmvJkyVDtUSh41"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "model.h"

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define RED_LED_PIN 4
#define BLUE_LED_PIN 5      
#define START_BUTTON_PIN 13
#define STOP_BUTTON_PIN 2
#define BUZZER_PIN 17
#define MIC_PIN 34          

LiquidCrystal_I2C lcd(0x27, 20, 4);

bool isRecording = false;
bool lastStartBtnState = HIGH;
bool lastStopBtnState = HIGH;
unsigned long recordingStartTime = 0;
unsigned long lastDisplayUpdate = 0;
bool blynkConnected = false;

#define VPIN_STATUS V0
#define VPIN_TIME V1
#define VPIN_RECORDING V3
#define VPIN_SOUND_LEVEL V4
#define VPIN_KEYWORDS V5
#define VPIN_CATEGORY V6


// Forward declarations
void beep(int times);
void checkButtons();
void updateDisplay();
void startRecording(String reason);
void stopRecording(String reason);
void classifyAndDisplay(const char* text);

void setup() {
  Serial.begin(115200);
  
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MIC_PIN, INPUT);
  
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
  
  // LCD Setup
  Wire.begin(21, 18);
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  delay(500);
  
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi...");
  
  // WiFi connect
  WiFi.begin(ssid, pass);
  int wifiTimeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTimeout < 30) {
    delay(500);
    wifiTimeout++;
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0, 2);
    lcd.print("WiFi OK! Blynk...");
    Serial.println("WiFi connected");
    
    Blynk.config(BLYNK_AUTH_TOKEN);
    blynkConnected = Blynk.connect(5000);
    
    if (blynkConnected) {
      lcd.setCursor(0, 3);
      lcd.print("Blynk OK!");
      Serial.println("Blynk connected");
    } else {
      lcd.setCursor(0, 3);
      lcd.print("Blynk failed");
      Serial.println("Blynk failed");
    }
  } else {
    lcd.setCursor(0, 2);
    lcd.print("WiFi failed!");
    Serial.println("WiFi failed");
  }
  
  delay(1000);
  
  digitalWrite(BLUE_LED_PIN, HIGH);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("=== SYSTEM READY ===");
  lcd.setCursor(0, 1);
  lcd.print("Blue btn: START");
  lcd.setCursor(0, 2);
  lcd.print("Red btn: STOP");
  lcd.setCursor(0, 3);
  if (blynkConnected) {
    lcd.print("Blynk: Connected");
  } else {
    lcd.print("Blynk: Offline");
  }
  
  beep(1);
  Serial.println("System Ready!");
  Serial.println("AI Model Loaded!");
  
  digitalWrite(BLUE_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
}

void loop() {
  if (blynkConnected) {
    Blynk.run();
  }
  
  checkButtons();
  updateDisplay();
}

void checkButtons() {
  bool startBtnState = digitalRead(START_BUTTON_PIN);
  if (startBtnState == LOW && lastStartBtnState == HIGH) {
    delay(50);
    if (!isRecording) {
      startRecording("Manual Start");
    }
  }
  lastStartBtnState = startBtnState;
  
  bool stopBtnState = digitalRead(STOP_BUTTON_PIN);
  if (stopBtnState == LOW && lastStopBtnState == HIGH) {
    delay(50);
    if (isRecording) {
      stopRecording("Manual Stop");
    }
  }
  lastStopBtnState = stopBtnState;
}

void startRecording(String reason) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start Recording...");
  lcd.setCursor(0, 1);
  lcd.print("Wait 5 secs...");
  
  beep(1);
  
  for (int i = 5; i >= 1; i--) {
    lcd.setCursor(0, 2);
    lcd.print("    ");
    lcd.setCursor(0, 2);
    lcd.print(i);
    lcd.print("...");
    
    digitalWrite(RED_LED_PIN, HIGH);
    delay(500);
    digitalWrite(RED_LED_PIN, LOW);
    delay(500);
  }
  
  isRecording = true;
  recordingStartTime = millis();
  
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BLUE_LED_PIN, LOW);
  
  beep(2);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("** RECORDING **");
  lcd.setCursor(0, 2);
  lcd.print(reason);
  lcd.setCursor(0, 3);
  lcd.print("Red btn: STOP");
  
  if (blynkConnected) {
    Blynk.virtualWrite(VPIN_STATUS, "Recording...");
    Blynk.virtualWrite(VPIN_RECORDING, 1);
  }
  
  Serial.println("Recording Started!");
}


// AI CLASSIFY AND DISPLAY FUNCTION
void classifyAndDisplay(const char* text) {
  // Get keywords
  const char* keywords[MAX_KEYWORDS];
  int numKeywords = extractKeywords(text, keywords, 3);
  
  // Get category
  int cat = predictCategory(text);
  const char* category = CATEGORIES[cat];
  
  // Build keywords string
  String keywordStr = "";
  for (int i = 0; i < numKeywords; i++) {
    if (i > 0) keywordStr += ", ";
    keywordStr += keywords[i];
  }
  
  // Serial output
  Serial.println("----------------------------------------");
  Serial.print("Keywords: ");
  Serial.println(keywordStr);
  Serial.print("Category: ");
  Serial.println(category);
  Serial.println("----------------------------------------");
  
  // LCD output
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Keywords:");
  lcd.setCursor(0, 1);
  lcd.print(keywordStr.substring(0, 20));  // Limit to LCD width
  lcd.setCursor(0, 2);
  lcd.print("Category:");
  lcd.setCursor(0, 3);
  lcd.print(category);
  
  // Blynk output - CORRECTED ORDER
  if (blynkConnected) {
    Blynk.virtualWrite(VPIN_CATEGORY, category);      // V5 gets category
    Blynk.virtualWrite(VPIN_KEYWORDS, keywordStr);    // V6 gets keywords
  }
}

void stopRecording(String reason) {
  isRecording = false;
  
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, HIGH);
  
  beep(2);
  
  unsigned long duration = (millis() - recordingStartTime) / 1000;
  int minutes = duration / 60;
  int seconds = duration % 60;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Recording Stopped");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(minutes);
  lcd.print("m ");
  lcd.print(seconds);
  lcd.print("s");
  lcd.setCursor(0, 2);
  lcd.print("Analyzing audio...");
  
  delay(2000);
  

  // AI AUTO-CLASSIFY AFTER RECORDING
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AI Processing...");
  
  delay(1500);
  
  // Simulated lecture content (replace with real audio-to-text later)
  const char* lectureContent = "Artificial intelligence and machine learning technology in classroom";
  
  // Classify and display results
  classifyAndDisplay(lectureContent);
  
  // Show recording info
  Serial.print("Duration: ");
  Serial.print(minutes);
  Serial.print("m ");
  Serial.print(seconds);
  Serial.println("s");
  
  if (blynkConnected) {
    Blynk.virtualWrite(VPIN_STATUS, "Saved");
    Blynk.virtualWrite(VPIN_TIME, String(minutes) + "m " + String(seconds) + "s");
    Blynk.virtualWrite(VPIN_RECORDING, 0);
  }
  
  beep(1);
}

void updateDisplay() {
  int soundLevel = analogRead(MIC_PIN);
  int soundPercent = map(soundLevel, 0, 4095, 0, 100);
  
  if (isRecording && millis() - lastDisplayUpdate > 500) {
    lastDisplayUpdate = millis();
    
    unsigned long duration = (millis() - recordingStartTime) / 1000;
    int minutes = duration / 60;
    int seconds = duration % 60;
    
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    if (minutes < 10) lcd.print("0");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) lcd.print("0");
    lcd.print(seconds);
    
    lcd.setCursor(0, 2);
    lcd.print("Sound: ");
    lcd.print(soundPercent);
    lcd.print("%   ");
    
    if (blynkConnected) {
      Blynk.virtualWrite(VPIN_TIME, String(minutes) + ":" + String(seconds));
      Blynk.virtualWrite(VPIN_SOUND_LEVEL, soundLevel);
    }
  }
}

void beep(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < times - 1) delay(100);
  }
}

// Blynk buttons
BLYNK_WRITE(V0) {
  if (param.asInt() == 1) {
    if (!isRecording) {
      startRecording("Blynk App");
    }
  }
}

BLYNK_WRITE(V4) {
  if (param.asInt() == 1) {
    if (isRecording) {
      stopRecording("Blynk App");
    }
  }
}
