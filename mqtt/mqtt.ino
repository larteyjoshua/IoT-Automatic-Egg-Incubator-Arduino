
#include <WiFi.h>
#include <PubSubClient.h>

float t;
float h;
// Replace the next variables with your SSID/Password combination
const char* ssid = "WorkSHop";
const char* password = "inf12345";
//m,./@1234
// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "mqtt.dioty.co";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
void setup() {
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
    if(messageTemp == "on"){
      Serial.println("on");
    //call function here
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      //call function here
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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();



  
 
  long now = millis();
  if (now - lastMsg > 15000) {
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
