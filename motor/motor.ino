/* Sensors */
const int motor = 19;

/* Two "independant" timed events */
const long eventTime_1 = 25000; //in ms
const long eventTime_2 = 120000; //in ms

/* When did they start the race? */
unsigned long previousTime_1 = 0;
unsigned long previousTime_2 = 0;

void setup() {
  pinMode(motor, OUTPUT);
  Serial.begin(9600);
}

void loop() {
turningegg();
}
void turningegg(){
 /* Updates frequently */
  unsigned long currentTime = millis();

  /* This is my event_1 */
  if ( currentTime - previousTime_1 >= eventTime_1) {
    Serial.println("Egg Rotating");
     digitalWrite(motor, HIGH);
    /* Update the timing for the next event*/
    previousTime_1 = currentTime;
  }

    /* This is my event_2 */
  else if ( currentTime - previousTime_2 >= eventTime_2) {

    Serial.println("Motor off for 8 hours");
    digitalWrite(motor, LOW);
    /* Update the timing for the next event*/
    previousTime_2 = currentTime;
  }
}
