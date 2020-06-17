#include "DHT.h"
#define DHTPIN 18     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 18 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
float h;
float t;

//relay initialization
int bulb = 19; //bulb
int fan = 23; //fan
int motor = 13; //turning egg

void setup() {
 pinMode(bulb, OUTPUT);
 pinMode(fan, OUTPUT);
 pinMode(motor, OUTPUT);
 Serial.begin(115200);
  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  getreadings();
  delay(2000);
  
 tempcontrol();
 Humicontrol();
 turningegg();
}
void getreadings(){
  
   // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   h = dht.readHumidity();
  // Read temperature as Celsius (the default)
   t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  
  }
  
  void tempcontrol(){
  digitalWrite(bulb, HIGH);
  if (t>40){
    digitalWrite(bulb, LOW);
    }
    if (t<38){
    digitalWrite(bulb, HIGH);
   }
  }
  
void Humicontrol(){
  digitalWrite(fan, LOW);
 if (h>65){
   digitalWrite(fan, HIGH);
  }
   if (t<51){
    digitalWrite(bulb, LOW);
   }
  }

void turningegg(){
  digitalWrite(motor, HIGH);
  delay(19000000);
  digitalWrite(motor, LOW);
  delay(60000000);
  }
