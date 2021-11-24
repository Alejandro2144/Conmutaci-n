#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Servo.h>
 
// ---------------------Global vars------------------------------
 
int contconexion = 0;
const char *ssid = "IoT-B19";
const char *password = "lcontrol2020*";
 
int LED = D3;
int MOTOR = D1;
int SUICHE = D2;
int servo = 0;
Servo mot;


 
WiFiClient client;
char JSONmessageBuffer[300];
 
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(SUICHE, INPUT);
 
  mot.attach(MOTOR);
  Serial.begin(115200);
  Serial.println("");
  //Serial.println(digitalRead(SWITCH)); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && contconexion < 50)
  {
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion < 50)
  {
    IPAddress ip(10, 201, 19, 10);
    IPAddress gateway(10, 201, 19, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(ip, gateway, subnet);
 
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("");
    Serial.println("Error de conexion");
  }
}

void writeJSON()
{
 
  StaticJsonBuffer<300> JSONbuffer; //Declaring static JSON buffer
  JsonObject &JSONencoder = JSONbuffer.createObject();
  Serial.print("suiche: ");
  Serial.println(digitalRead(SUICHE));
  JSONencoder["suiche"] = digitalRead(SUICHE) == HIGH;
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
}
 

void sendRequest()
{
  //    Serial.println(JSONmessageBuffer);
  writeJSON();
  HTTPClient http;                                                //Declare object of class HTTPClient
  http.begin(client, "http://10.201.19.145:10000/write"); //Specify request destination
  http.addHeader("Content-Type", "application/json");             //Specify content-type header
 
  int httpCode = http.POST(JSONmessageBuffer); //Send the request
  String payload = http.getString();           //Get the response payload
 
  if (httpCode == 200)
  {
    // Allocate JsonBuffer
    // Use arduinojson.org/assistant to compute the capacity.
    const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
    DynamicJsonBuffer jsonBuffer(capacity);
 
    // Parse JSON object
    JsonObject &root = jsonBuffer.parseObject(payload);
    if (!root.success())
    {
      Serial.println(F("Parsing failed!"));
      return;
    }
 
    // Decode JSON/Extract values
    Serial.println(F("Response:"));
   Serial.println(root["led"].as<char *>());
    Serial.println(root["servomotor"].as<char *>());
    mot.write(root["servomotor"]);
    digitalWrite(LED, root["led"]);
  }
  else
  {
    Serial.println("Error in response");
  }
  Serial.println(httpCode); //Print HTTP return code
  Serial.println(payload);  //Print request response payload
  http.end();               //Close connection
 
  delay(3000);
}
 
void loop()
{
  

  
  //if (WiFi.status() == WL_CONNECTED) {
  sendRequest();
  //  } else {
  //    Serial.println("Error in WiFi connection");
  // }
}
