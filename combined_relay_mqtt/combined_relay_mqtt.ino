
#include <WiFi.h>
#include <PubSubClient.h>
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

// Replace the next variables with your SSID/Password combination
const char* ssid = "SuperJosh";
const char* password = "@super1234";
//m,./@1234
// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "mqtt.dioty.co";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup() {
  pinMode(bulb, OUTPUT);
 pinMode(fan, OUTPUT);
 pinMode(motor, OUTPUT);
 dht.begin();
 
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
  void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

// Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the /larteyjoshua@gmail.com/test, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "/larteyjoshua@gmail.com/test") {
    Serial.print("Changing output to ");
    if(messageTemp == "heaton"){
      Serial.println("bulb is on");
       bulb_on();
    }
    else if(messageTemp == "heatoff"){
      Serial.println("bulb is off");
      bulb_off();
    }
    else if(messageTemp == "airoff"){
      Serial.println("fan is off");
      fan_off();
    }
    else if(messageTemp == "airon"){
      Serial.println("fan is on");
      fan_off();
    }
  }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
     // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),"larteyjoshua@gmail.com","7f8a9110")) {
      Serial.println("connected");
      client.publish("/larteyjoshua@gmail.com/boardstatus", "ESP BOARD CONNECTED TO THE SERVER");
      // Subscribe
      client.subscribe("/larteyjoshua@gmail.com/test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
turningegg();
  // Wait a few seconds between measurements.
  getreadings();
  delay(2000);
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
  
 
 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
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
      client.publish("/larteyjoshua@gmail.com/SensorData", sensorReading);
      Serial.println(sensorReading);
  }

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
