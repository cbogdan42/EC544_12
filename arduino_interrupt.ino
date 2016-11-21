const byte interruptPin = 2;
int flag = 0;

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
  Serial.begin(9600);
}

void loop() {
  if(flag)
  {
    Serial.println("In");
    flag = 0;
  }
}

void blink() {
  flag = 1;
}
