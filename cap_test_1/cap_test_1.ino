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
int threshold = 50;   // Detection threshold
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
  for (int i = 0; i < 50; i++) {
    total += analogRead(TOUCH_PIN);
    delay(10);
  }
  baseline = total / 50;
  
  Serial.println("Calibration complete!");
  Serial.println("Baseline: " + String(baseline));
}

static void setRGB(uint8_t fixtureIndex, uint8_t val) {
  // Sets R, G, B = val for fixtureIndex [0..5]
  uint16_t base = fixtureIndex * CH_PER_RGB + 1;
  dmxTx.set(base    , val);
  dmxTx.set(base + 1, val);
  dmxTx.set(base + 2, val);
}

void loop() {
  // Read the analog pin
  int value = analogRead(TOUCH_PIN);
  int difference = abs(value - baseline);
  
  // Check if touched (value changes significantly from baseline)
  if (difference > threshold && !touched) {
    Serial.println("TOUCHED! Value: " + String(value) + ", Diff: " + String(difference));
    touched = true;
    digitalWriteFast(ledPin, LOW); // CORRECTED: Consistent use of digitalWriteFast
    
    // 1) Fixtures 1 & 6 → full white
    setRGB(0, 255);
    setRGB(5, 255);  // CORRECTED: Index 5 is the 6th fixture (0-based indexing)

    delay(500);

    // 2) Fixtures 2 & 5 → full white
    setRGB(1, 255);
    setRGB(4, 255);

    delay(250);

    // 3) Fixtures 3 & 4 → full white
    setRGB(2, 255);
    setRGB(3, 255);  // CORRECTED: 4th fixture is index 3

    // 4) Fade last fixture upwards over 1 second
    uint16_t lastCh = NUM_RGB_FIX * CH_PER_RGB + 1;
    for (uint16_t v = 0; v <= 255; v++) {
      dmxTx.set(lastCh, v);
      delay(4); // CORRECTED: 4ms is closer to 1s/255 than 1000/255
    }
    
    // ISSUE: This line uses channel 20 directly instead of calculated lastCh value
    // dmxTx.set(20, 255);  // Should be using lastCh instead of hard-coded 20
    dmxTx.set(lastCh, 255); // CORRECTED: Use calculated channel

    delay(7000);
    
    // ISSUE: Similarly, should use lastCh
    // dmxTx.set(20, 0);
    dmxTx.set(lastCh, 0); // CORRECTED: Use calculated channel
    
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