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

const int motor = 19;
const int fan = 23;
const int bulb = 12;

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
  if (t>40)
  {
    bulb_off();
  }
  if (t<36)
  {
    bulb_on();
  }
  if (h>56)
  {
    fan_on();
  }
  if (h<49)
  {
    fan_off();
  }
  
  

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
//  digitalWrite(motor, HIGH);
//  delay(19000000);
//  digitalWrite(motor, LOW);
//  delay(60000000);
  }
