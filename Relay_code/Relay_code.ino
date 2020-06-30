/* Two "independant" timed events */
const long eventTime_1 = 25000; //in ms
const long eventTime_2 = 120000; //in ms

/* When did they start the race? */
unsigned long previousTime_1 = 0;
unsigned long previousTime_2 = 0;

const int motor = 19;
const int fan = 13;
const int bulb = 12;
long lastMsg = 0;

void setup() {
 pinMode(bulb, OUTPUT);
 pinMode(fan, OUTPUT);
 pinMode(motor, OUTPUT);
 Serial.begin(115200);
}

 void bulb_on(){
  digitalWrite(bulb, HIGH);
  }
  void bulb_off(){
  digitalWrite(bulb, LOW);
  }
  
  void fan_on(){
  digitalWrite(fan, HIGH);
  }
 void fan_off(){
  digitalWrite(fan, LOW);
  }

void turningegg(){
 /* Updates frequently */
  unsigned long currentTime = millis();

  /* This is my event_1 */
  if ( currentTime - previousTime_1 >= eventTime_2) {
    Serial.println("Egg Rotating");
     digitalWrite(motor, LOW);
    /* Update the timing for the next event*/
    previousTime_1 = currentTime;
  }
    /* This is my event_2 */
  else if ( currentTime - previousTime_2 >= eventTime_1) {

    Serial.println("Motor off for 8 hours");
    digitalWrite(motor, HIGH);
    /* Update the timing for the next event*/
    previousTime_2 = currentTime;
  }
}
void loop(){
  turningegg();
  delay(200);
  bulb_on();
  delay(200);
  fan_on();
  delay(200);
  fan_off();
}
  
