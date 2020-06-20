
#include <arduino-timer.h>

const int motor = 19;
const int fan = 23;
const int bulb = 12;
int period = 18000;
unsigned long time_now = 0;
auto timer = timer_create_default(); // create a timer with default settings
void setup() {
  pinMode(bulb, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(motor, OUTPUT);
  timer.every(50000, turningegg);
}
void loop() {
   timer.tick(); // tick the timer
  //tempcontrol();
  //Humicontrol();
}

void tempcontrol() {
  digitalWrite(bulb, HIGH);
  delay(200);
  digitalWrite(bulb, LOW);
  delay(300);
}

void Humicontrol() {
  digitalWrite(fan, HIGH);
  delay(300);
  digitalWrite(fan, LOW);
  delay(300);
}

bool turningegg(void *) {

  if (millis() >= time_now + period) {
    time_now += period;
    Serial.println("Egg Rotating");
    digitalWrite(motor, LOW);
  }
   return true;
 

}
