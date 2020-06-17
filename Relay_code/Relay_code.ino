int bulb = 19;
int fan = 23;
int motor = 13;
void setup() {
 pinMode(bulb, OUTPUT);
 pinMode(fan, OUTPUT);
 pinMode(motor, OUTPUT);
 
}
void loop() {
 tempcontrol();
 Humicontrol();
 turningegg();
}

void tempcontrol(){
  digitalWrite(bulb, HIGH);
  delay(3000);
  digitalWrite(bulb, LOW);
  delay(3000);
  }
  
void Humicontrol(){
  digitalWrite(fan, HIGH);
  delay(3000);
  digitalWrite(fan, LOW);
  delay(3000);
  }

void turningegg(){
  digitalWrite(motor, HIGH);
  delay(3000);
  digitalWrite(motor, LOW);
  delay(3000);
  }
