#define output_pin 7
#define input_pin 8 

void setup() {
  pinMode(output_pin, OUTPUT);
  pinMode(input_pin, INPUT_PULLDOWN);
  digitalWrite(output_pin, HIGH);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(input_pin) == HIGH){
    Serial.println("Touched!");
    delay(1000);
  }
}
