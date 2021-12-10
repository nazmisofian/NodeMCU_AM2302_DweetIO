/*
 * reference: Examples > ESP8266WiFi > WiFiClient 
 */

#include <ESP8266WiFi.h>
#include "DHT.h"
// define DHTTYPE DHT11
// define DHTTYPE DHT22
#define DHTTYPE DHT22

//const int AnalogIn  = A0;
//const int AnalogIn = A1;
//const int SensorIn = D7;
//const int SensonIn2 = D8;

const char* ssid     = "ELV_ARUBA";
const char* password = "s0rryn0wifi";

const char* host = "www.dweet.io";
const char* thing  = "JN_Sensor";
const char* thing_content = "Temperature_Celsius";
const char* thing_content_2 = "Humidity_Relative_Humidity";

//DHT Sensor
uint8_t DHTPin = D7;

// Initialize DHT Sensor
DHT dht (DHTPin, DHTTYPE);

float temperature;
float humidity;

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(DHTPin, INPUT);
  dht.begin();
  // We start by connecting to a WiFi network

  Serial.println();
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

//int value = 0;

void Incoming_Sensor_Reading(){
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  Serial.println("Temperature = ");
  Serial.println(temperature);
}

void loop() {
  delay(1000);
  //value = analogRead(AnalogIn);
  //value = digitalRead(SensorIn);
  //value2 = analogRead(AnalogIn2);
  //value2 = digitalRead(SensorIn2);
  
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  Incoming_Sensor_Reading();
  String url = "/dweet/for/";
  url += thing;
  url += "?";
  url += thing_content;
  url += "=";
  url += temperature;
  url += "&";
  url += thing_content_2;
  url += "=";
  url += humidity;
  
  /*
  url += thing;
  url += "?";
  url += thing_content;
  url += "=";
  url += value;
  */
  
  /*
  url += thing_content_2;
  url += "=";
  url += value2;
  */
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}
