// Touch sensing for Teensy 4.0 using analog input
// No additional components required, but less reliable than dedicated touch sensors

#define TOUCH_PIN 21  // Any analog pin (A0-A9 on Teensy 4.0)
#define LED_PIN 13    // Built-in LED

// Variables
int baseline = 0;     // Baseline reading
int threshold = 50;   // Detection threshold
bool touched = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("Teensy 4.0 Touch Detection");
  Serial.println("-------------------------");
  Serial.println("Calibrating - don't touch the pin");
  
  // Calibrate baseline
  long total = 0;
  for (int i = 0; i < 20; i++) {
    total += analogRead(TOUCH_PIN);
    delay(10);
  }
  baseline = total / 20;
  
  Serial.println("Calibration complete!");
  Serial.println("Baseline: " + String(baseline));
}

void loop() {
  // Read the analog pin
  int value = analogRead(TOUCH_PIN);
  int difference = abs(value - baseline);
  
  // Check if touched (value changes significantly from baseline)
  if (difference > threshold && !touched) {
    Serial.println("TOUCHED! Value: " + String(value) + ", Diff: " + String(difference));
    digitalWrite(LED_PIN, HIGH);
    touched = true;
  } 
  else if (difference <= threshold && touched) {
    Serial.println("Released");
    digitalWrite(LED_PIN, LOW);
    touched = false;
  }
  
  // Optional debugging output
  // Serial.println("Value: " + String(value) + ", Diff: " + String(difference));
  
  delay(500);
}