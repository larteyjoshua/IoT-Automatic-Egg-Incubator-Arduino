
#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <AsyncMqttClient.h>
#include "DHT.h"
#define DHTPIN 17     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
RTC_DS1307 rtc;
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 18 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
float h;
float t;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
String dataMessage;

const int motor = 16;
const int fan = 27;
const int bulb = 14;
LiquidCrystal_I2C lcd(0x27, 16, 2);
long lastMsg = 0;
char msg[50];
// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;

// Change the credentials below, so your ESP32 connects to your router
#define WIFI_SSID "SuperJosh"
#define WIFI_PASSWORD "useyourdata"




// Create objects to handle MQTT client
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

// Add more topics that want your ESP32 to be subscribed to
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  // ESP32 subscribed to esp32/led topic
  uint16_t packetIdSub = mqttClient.subscribe("/larteyjoshua@gmail.com/test", 0);
  Serial.print("Subscribing at QoS 0, packetId: ");
  Serial.println(packetIdSub);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

// You can modify this function to handle what happens when you receive a certain message in a specific topic
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  String messageTemp;
  for (int i = 0; i < len; i++) {
    //Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  // Check if the MQTT message was received on topic test
  if (strcmp(topic, "/larteyjoshua@gmail.com/test") == 0) {
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
      fan_on();
    }
   else if(messageTemp == "getdata"){
      Serial.println("reading data");
      readFile(SD, "/incubatordata.txt");
    
    }
    else if(messageTemp == "delete"){
      Serial.println("deletingdata data");
      deleteandcreate();
    }
  }
 
  Serial.println("Publish received.");
  Serial.print("  message: ");
  Serial.println(messageTemp);
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
    Serial.write(file.read());
   String datavalues = (file.readString());

//        // Convert the value to a char array
//      char dataString[1000];
//      dtostrf( (file.readString()), 1, 2, dataString);
      char dataReading[10000];
//      String dataValues= String(dataString);
       datavalues.toCharArray(dataReading, (datavalues.length() + 1));
        uint16_t packetIdPub2 = mqttClient.publish("/larteyjoshua@gmail.com/incubatordata", 2, true, dataReading);
    }
    file.close();
}
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    File file2 = fs.open(path);
    if (file2.available()){
      Serial.println("File exist");
    }
    else if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void setup() {
  pinMode(bulb, OUTPUT);
 pinMode(fan, OUTPUT);
 pinMode(motor, OUTPUT);
 dht.begin();
  Serial.begin(115200);
  lcd.begin(); // sixteen characters across - 2 lines
  lcd.backlight();

if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

 writeFile(SD, "/incubatordata.txt", " ID,Timestamp,Temperature,Humidity  \r\n");


  
  
if (! rtc.begin()) 
  {
    Serial.print("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) 
  {
    Serial.print("RTC is NOT running!");
  }
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
    rtc.adjust(DateTime(2020, 7, 8, 11, 57, 0));// to set the time manualy 


  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer("mqtt.dioty.co", 1883);
  mqttClient.setCredentials("larteyjoshua@gmail.com","7f8a9110");

  connectToWifi();
}

void loop() {

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

 DateTime now = rtc.now();
    Serial.print(F("TIME"));
    Serial.print(" ");
    Serial.print(now.hour());
    Serial.print(':');
    Serial.print(now.minute());
    Serial.print(':');
    Serial.print(now.second());
    Serial.print("  ");
    
    delay(3000);
   

   
    Serial.print(F("DATE"));
    Serial.print(" ");
    //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    //Serial.print(" ");
    Serial.print(now.day());
    Serial.print('/');
    Serial.print(now.month());
    Serial.print('/');
    Serial.print(now.year());
    Serial.print("  ");
    
  
//    if ((now.hour()==18) && (now.minute()==36)){
//      
//       turningegg();
//      }
//      if ((now.hour()==18) && (now.minute()==37)){
//      
//       digitalWrite(motor, LOW);
//      }


  if ((now.hour()==19) && (now.minute()==1)){
       digitalWrite(motor, HIGH);
      }
  if ((now.hour()==19) && (now.minute()==2)){
       digitalWrite(motor, LOW);
      }
  if ((now.hour()==3) && (now.minute()==1)){
       digitalWrite(motor, HIGH);
      }
  if ((now.hour()==3) && (now.minute()==2)){
       digitalWrite(motor, LOW);
      }

   if ((now.hour()==11) && (now.minute()==1)){
       digitalWrite(motor, HIGH);
      }
      
  if ((now.hour()==11) && (now.minute()==2)){
       digitalWrite(motor, LOW);
      }
   
  long time = millis();
  if (time - lastMsg > 30000) {
    lastMsg = time;
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
       // Convert the value to a char array
       
     
      char sensorReading[90];
      String sensorValues = "{ \"temperature\": \"" + String(TempString) + "\", \"humidity\" : \"" + String(HumiString) + "\"}";
      sensorValues.toCharArray(sensorReading, (sensorValues.length() + 1));
     uint16_t packetIdPub2 = mqttClient.publish("/larteyjoshua@gmail.com/SensorData", 2, true, sensorReading);
    Serial.print("Publishing on topic esp32/SensorData at QoS 2, packetId: ");
    Serial.println(packetIdPub2);
      lcd.setCursor(0,0);
      lcd.print("Temp: ");
       lcd.print(TempString);
       lcd.print("C");
     // 8th character - 2nd line 
       lcd.setCursor(0,1);
       lcd.print("Humi: ");
       lcd.print(HumiString);
       lcd.print("%");
        readingID++;

        char Timedate[20];
sprintf(Timedate, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
Serial.print(F("Date/Time: "));
Serial.println(Timedate);
     dataMessage = String(readingID) + "," + String(Timedate) + "," + String(TempString) + "," + String(HumiString) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/incubatordata.txt", dataMessage.c_str());
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
  void deleteandcreate(){
    deleteFile(SD, "/incubatordata.txt");
    writeFile(SD, "/incubatordata.txt", " ID,Timestamp,Temperature,Humidity \r\n");
    }
  
