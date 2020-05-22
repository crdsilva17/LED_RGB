#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SSID  "Virus" // Ssid Network
#define PASSWD  "aoc18090" // Password for Network

int pinR = 12; //pin red color
int pinG = 13; //pin green color
int pinB = 14; //pin blue color


String page = ""; // variable that we can set the html page

ESP8266WebServer serverA(80); //Instance Server

const char* user = "admin"; //username
const char* pass = "senha123"; //password




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinR, OUTPUT); // Set pin as Output
  pinMode(pinG, OUTPUT); // Set pin as Output
  pinMode(pinB, OUTPUT); // Set pin as Output

  analogWriteRange(8); // Range 0 at 255
  analogWriteFreq(5000); // Set Frequency

  analogWrite(pinR,0); // pin off
  analogWrite(pinG,0); // pin off
  analogWrite(pinB,0); //pin off

  WiFi.mode(WIFI_STA); // Station mode
  WiFi.begin(SSID,PASSWD); //Connect network
  Serial.print("Connect ");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.println("Connected!");
  Serial.println(WiFi.localIP());

  serverA.on("/", [](){
    if(!serverA.authenticate(user,pass)){
      return serverA.requestAuthentication();
    }
    serverA.send(200,"text/plain","Login Ok!");
  });
  

  serverA.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  serverA.handleClient();
}