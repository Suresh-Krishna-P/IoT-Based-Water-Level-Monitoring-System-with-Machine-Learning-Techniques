#define BLYNK_TEMPLATE_ID "TMPL3iuoVeCd7"
#define BLYNK_TEMPLATE_NAME "Water level monitoring system"
#define BLYNK_PRINT Serial  // This enables Blynk debug information to Serial

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Ultrasonic Sensor Pins
#define trig 2
#define echo 4

// Relay Pin - Try a different pin with better current capability
// #define relay 26  // Original pin
#define relay 12    // Try this pin instead

int MaxLevel = 32;

BlynkTimer timer;B  

char auth[] = "iHtBL531NQeXKZSeSOBOQD-JChO02Xq3";
char ssid[] = "OPPO A78 5G";
char pass[] = "12345678";

void setup() {
  Serial.begin(115200);
  Serial.println("System starting...");
  
  // Ensure relay is initialized before connecting to Blynk
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay, OUTPUT);
  // Try with internal pull-up resistor
  // pinMode(relay, OUTPUT_OPEN_DRAIN);
  // or
  // pinMode(relay, INPUT_PULLUP); digitalWrite(relay, HIGH);
  
  digitalWrite(relay, HIGH);  // Motor OFF initially
  Serial.println("Relay initialized to OFF state");
  
  // Connect to Blynk with additional debug info
  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.println("Connected to Blynk!");
  
  // Check water level every 2 seconds
  timer.setInterval(2000L, ultrasonic);
  
  // Test relay at startup (uncomment to test)
  // testRelay();
}

// Optional relay test function
void testRelay() {
  Serial.println("Testing relay...");
  digitalWrite(relay, LOW);  // Turn ON
  Serial.println("Relay should be ON now");
  delay(2000);
  digitalWrite(relay, HIGH); // Turn OFF
  Serial.println("Relay should be OFF now");
}

// Measure water level and send to Blynk (V0)
void ultrasonic() {
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long t = pulseIn(echo, HIGH);
  int distance = t / 29 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  int waterLevel = MaxLevel - distance; // Water height
  if (waterLevel < 0) waterLevel = 0;

  Blynk.virtualWrite(V0, waterLevel); // Update Blynk
}

// Motor ON/OFF from Blynk app (V1 button)
BLYNK_WRITE(V1) {
  int state = param.asInt();
  Serial.print("Blynk button V1 pressed with state: ");
  Serial.println(state);
  
  if (state == 1) {
    Serial.println("Turning relay ON...");
    digitalWrite(relay, LOW);  // Motor ON
    Serial.println("Relay pin set to LOW (should be ON)");
  } else {
    Serial.println("Turning relay OFF...");
    digitalWrite(relay, HIGH); // Motor OFF
    Serial.println("Relay pin set to HIGH (should be OFF)");
  }
  
  // Read back the pin state for verification
  int currentState = digitalRead(relay);
  Serial.print("Current relay pin state: ");
  Serial.println(currentState);
}

// Ensure Blynk sync on reconnection
BLYNK_CONNECTED() {
  Serial.println("Blynk connected! Syncing virtual pins...");
  Blynk.syncVirtual(V1);  // Sync the relay state
}

void loop() {
  Blynk.run();
  timer.run();
}
