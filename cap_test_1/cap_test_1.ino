// Touch sensing for Teensy 4.0 using analog input with DMX control
// Issues corrected and noted with comments

#include <TeensyDMX.h>

// Use the TeensyDMX "Sender" on Serial1 (DMX TX on pin 1)
namespace teensydmx = ::qindesign::teensydmx;
teensydmx::Sender dmxTx{Serial1};

const uint8_t ledPin           = 13;
const uint8_t NUM_RGB_FIX      = 7;    // six RGB fixtures
const uint8_t CH_PER_RGB       = 3;    // each uses R,G,B
const uint8_t CH_LAST_FIX      = 1;    // one single-channel fixture at the end
const uint16_t TOTAL_DMX_CH    = NUM_RGB_FIX * CH_PER_RGB + CH_LAST_FIX;

#define TOUCH_PIN A7  // CORRECTED: Use A7 for pin 21 (analog input)
#define LED_PIN 13    // Built-in LED (duplicate of ledPin above)

// Variables
int baseline = 0;     // Baseline reading
int threshold = 80;   // Detection threshold
bool touched = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWriteFast(ledPin, HIGH); // CORRECTED: Initialize LED state
  Serial.begin(9600);
  delay(1000);
  
  dmxTx.begin();

  allOff();

  Serial.println("Teensy 4.0 Touch Detection");
  Serial.println("-------------------------");
  Serial.println("Calibrating - don't touch the pin");
  
  // Calibrate baseline
  long total = 0;
  for (int i = 0; i < 100; i++) {
    total += analogRead(TOUCH_PIN);
    delay(10);
  }
  baseline = total / 100;
  
  Serial.println("Calibration complete!");
  Serial.println("Baseline: " + String(baseline));
}

static void setRGB(uint8_t fixtureIndex, uint8_t val) {
  uint16_t base = fixtureIndex * CH_PER_RGB + 1;
  dmxTx.set(base    , val);
  dmxTx.set(base + 1, val);
  dmxTx.set(base + 2, val);
}

void loop() {
  int value = analogRead(TOUCH_PIN);
  int difference = abs(value - baseline);
  
  if (difference > threshold && !touched) {
    Serial.println("TOUCHED! Value: " + String(value) + ", Diff: " + String(difference));
    touched = true;
    digitalWriteFast(ledPin, LOW);

    setRGB(0, 255);
    setRGB(5, 255);

    delay(500);

    setRGB(1, 255);
    setRGB(4, 255);

    delay(250);

    setRGB(2, 255);
    setRGB(3, 255);  // CORRECTED: 4th fixture is index 3
    
    delay(2000);

    dmxTx.set(20, 255);

    delay(7000);

    dmxTx.set(20, 0);
    
    allOff();
    digitalWriteFast(ledPin, HIGH);
  }

  // Reset on release
  else if (difference <= threshold && touched) {
    Serial.println("Released");
    digitalWriteFast(ledPin, HIGH); // CORRECTED: Consistent use of digitalWriteFast
    touched = false;
    allOff();
  }

  delay(50); // CORRECTED: 500ms is too slow for responsive touch sensing
}

void allOff(){
  for (uint16_t ch = 1; ch <= TOTAL_DMX_CH; ch++) {
    dmxTx.set(ch, 0);
  }
}