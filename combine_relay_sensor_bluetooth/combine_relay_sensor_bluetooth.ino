/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Load libraries
#include "BluetoothSerial.h"
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

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Bluetooth Serial object
BluetoothSerial SerialBT;

// Handle received and sent messages
String message = "";
char incomingChar[10];

void setup() {
 pinMode(bulb, OUTPUT);
 pinMode(fan, OUTPUT);
 pinMode(motor, OUTPUT);
 dht.begin();
 Serial.begin(115200);
  // Bluetooth device name
 SerialBT.begin("IoT Automatic Egg Incubator");
 Serial.println("The device started, now you can pair it with bluetooth!");
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

void loop(){ 
 getreadings();
  delay(2000);
   turningegg();
  if (t>39)
  {
    bulb_off();
  }
  if (t<37)
  {
    bulb_on();
  }
  if (h>55)
  {
    fan_on();
  }
  if (h<50)
  {
    fan_off();
  }
  
  long now = millis();
  if (now - lastMsg > 30000) {
    lastMsg = now;
    // Convert the value to a char array
   char TempString[8];
   dtostrf(t, 1, 2, TempString);
    Serial.print("Temperature: ");
    Serial.println(TempString);
    
    // Convert the value to a char array
      char HumiString[8];
      dtostrf( h, 1, 2, HumiString);
      Serial.print(" Humidity: ");
      Serial.println(HumiString);
      
     
      char sensorReading[90];
      String sensorValues = "{ \"temperature\": \"" + String(TempString) + "\", \"humidity\" : \"" + String(HumiString) + "\"}";
      sensorValues.toCharArray(sensorReading, (sensorValues.length() + 1));
      SerialBT.println( sensorReading);
      Serial.println(sensorReading);
  }
    
  // Read received messages (LED control command)
  if (SerialBT.available()){
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n'){
      message += String(incomingChar);
    }
    else{
      message = "";
    }
    Serial.write(incomingChar);  
  }
  // Check received message and control output accordingly
  if (message =="heaton"){
    Serial.println("bulb is on");
      bulb_on();
  }
  else if (message =="heatoff"){
     Serial.println("bulb is off");
      bulb_off();
  }
  else if(message == "airoff"){
      Serial.println("fan is off");
      fan_off();
    }
    else if(message == "airon"){
      Serial.println("fan is on");
      fan_off();
    }

  delay(20);
}


  
