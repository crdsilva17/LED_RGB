/**
 * @file main.cpp
 * @brief Controls pool pump and RGB led
 * 
 * 
 * @author Cristiano Rocha
 * @warning This source is only first implementation
 * @todo Implementation List
 *          - Turn on Led
 *          - OnOff Pump
 * @bug Bugs list
 * @copyright RC Automação all copyrights reserved
 * 
 * */


/*********************
 * LIBRARIES
 * ******************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

/******************************************************
 * DEFINES
 ******************************************************/


/**************************************************************
 * GLOBAL VARIABLES
 **************************************************************/
const char* user = "admin"; /*!< username for login*/
const char* pass = "senha123"; /*!< paasword for access*/
const char* host = "aqua"; /*!< hostname for local access*/
const char* update_path = "/firmware"; /*!< Path for update*/
const char* SSID = "Virus"; ///< Ssid Network
const char* PASSWD =  "aoc18090"; ///< Password for Network
bool hand = true; /*!< handle manual or automatic action*/
unsigned long setTime = 1000; /*!< set time delay */
unsigned long previusTime = 0; /*!< receive previus time for trigger delay */
unsigned long previusTimes = 0; /*!< receive previus time for trigger delay ntp */
File uploadFile; /*!< Store data for SPIFFS readed*/
int pinR = 12; /*!< Set pin red as number 12 */
int pinG = 13; /*!< Set pin green as number 13 */
int pinB = 14; /*!< Set pin blue as number 14 */
int pump = 5; ///< define pin of pump
int sunLight = 4; /// define ldr sensor
String page = ""; /*!< Store page HTML */
String ntpTime = ""; /*!< Store ntp time */
String prog1 = "0";
String prog2 = "0";


/******************************************************************
 * Classes
 *****************************************************************/
/**
 * @class httpUpdater ESP8266HTTPUpdateServer.h "inc/ESP8266HTTPUpdate.h"
 * @brief Update class
 * 
 * provide update for firmware on the http request
 * 
 * @see other classes
 *      - serverA
 *      - RGB
 */
  ESP8266HTTPUpdateServer httpUpdater; ///< INSTANCE FOR HTTP UPDATE
/**
 * @class serverA(80)
 * @brief WebServer Class
 * 
 * This create a object for handle request response of client
 * 
 * @param 80 - Web port
 * 
 * @see other classes
 *      - httpUpdater
 *      - RGB
 **/
  ESP8266WebServer serverA(80); 

/**
 * @class ntpUDP
 * @brief create client udp
 * 
 * */
WiFiUDP ntpUDP;

/**
 * @class ntp
 * @brief create client ntp
 * 
 * */
NTPClient ntp(ntpUDP,"b.st1.ntp.br", -3 * 3600);

/**
 * @struct RGB 
 * @brief Struct for Led
 * 
 * Generates struct for rgb color object
 * 
 * @see other classes
 *      - serverA
 *      - httpUpdater
 */
  struct RGB
  {
    ///RECEIVE BYTE COLOR RED
    byte r; 
    ///RECEIVE BYTE COLOR GREEN
    byte g; 
    
    byte b; /*!< RECEIVE BYTE COLOR BLUE*/
  };
  
RGB color = {0,0,0};/*!< object RGB color */


/***********************************************
 * FUNCTIONS
 **********************************************/
/**
 * @fn handleAction()
 * @brief handles customer request
 * 
 *        turn on led, turn on pump and all actions
 * 
 * @param - void 
 * @return void
 * */
void handleAction();
/**
 * @fn handleNotFound()
 * @brief handle error 404
 * 
 *        response for request error
 * 
 * @param - void
 * @return void
 * */
void handleNotFound();
/**
 * @fn led(RGB colorname)
 * @brief turn on led in the desired color
 * 
 *        receive color for turn on led
 * 
 * @param colorname - struct type {r,g,b} 
 * @return void
 * */
void led(RGB colorname);
/**
 * @fn wait()
 * @brief  generates delay without locking the code
 * 
 *          set delay with millis() function
 * 
 * @param - void
 * @return Boolean type
 * */
bool wait();

/**
 * @fn wait(unsigned long t)
 * @brief  generates delay without locking the code
 * 
 *          set delay with millis() function
 *          with parameter t
 * 
 * @param t - unsigned long
 * @return void
 * */
void wait(unsigned long t);
/**
 * @fn getContentType(String filename)
 * @brief Convert the file extension to the MIME type
 * 
 *        set of the request type for client 
 *        .html
 *        .css
 *        .js
 *        .ico
 * 
 * @param filename - String
 * @return String 
 * */
String getContentType(String filename); // convert the file extension to the MIME type

/**
 * @fn handleFileRead(String path)
 * @brief Send the right file to client(if it exists)
 *        
 *@param path - String
 *@return bool
 * */
bool handleFileRead(String path);       // send the right file to the client (if it exists)


/********************************************
 *  IMPLAMENTATION
 * *******************************************/
/**
 * @fn setup()
 * @brief Setting source
 * 
 *        put all configuration setup of device
 * */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinR, OUTPUT); /* Set pin red as Output */
  pinMode(pinG, OUTPUT); /* Set pin green as Output */
  pinMode(pinB, OUTPUT); /* Set pin blue as Output */
  pinMode(pump, OUTPUT); /* Set pin pump as Output */
  pinMode(sunLight, INPUT); /* Set pin sensor sun as Input */

  digitalWrite(pump, LOW); /* Turn pump to off */
  digitalWrite(pinR,LOW); /* Turn off pin red */
  digitalWrite(pinG,LOW); /* Turn off pin green */
  digitalWrite(pinB,LOW); /* Turn off pin blue */

  analogWriteRange(8); /* PWM Range 0 at 255 */
  analogWriteFreq(300); /* Set Frequency to 300 hz */

  WiFi.mode(WIFI_STA); /* Set device as Station mode */
  WiFi.begin(SSID,PASSWD); /* Try connect to network */
  Serial.print("Connecting ");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected.");
  Serial.println(WiFi.localIP().toString());
  serverA.on("/", [](){
    if(!serverA.authenticate(user,pass)){ 
      return serverA.requestAuthentication(); 
    }
    //handleRoot(); 
        if (!handleFileRead(serverA.uri()))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  
  serverA.on("/update",handleAction); 

  serverA.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(serverA.uri()))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  
  MDNS.begin(host);
  httpUpdater.setup(&serverA, update_path, user, pass);
  serverA.begin();

  ntp.begin();

  MDNS.addService("http", "tcp", 80);

}



void loop() {
  
  if((hand != true) && (digitalRead(sunLight))){
    bool delay = wait();
    led(color);
    if(delay){
      if(color.r >= 15 && color.g < 15 && color.b == 0){
        color.g++;
      }else if(color.r > 0 && color.g >= 15 && color.b == 0){
        color.r--;
      }else if(color.r == 0 && color.b < 15 && color.g >= 15){
        color.b++;
      }else if(color.b >= 15 && color.g > 0 && color.r == 0){
        color.g--;
      }else if(color.b >= 15 && color.r < 15 && color.g == 0){
        color.r++;
      }else if(color.b > 0 && color.g == 0 && color.r >= 15){
        color.b--;
      }
    }
    }
  
  if((prog1 == ntpTime)&&(digitalRead(pump) == LOW)){
    Serial.println("Pump on");
    digitalWrite(pump, HIGH);
  }
  if((prog2 == ntpTime)&&(digitalRead(pump) == HIGH)){
    Serial.println("Pump off");
    digitalWrite(pump, LOW);
  }
  
  serverA.handleClient();
  MDNS.update();
  wait(1000L);

}
/**
 * Wait for set time
 **/
bool wait(){
  unsigned long currentTime = millis();
  if((currentTime - previusTime) >= setTime){
    previusTime = currentTime;
    return true;
  }else{
    return false;
  }
  
}

/**
 * Wait for set time
 **/
void wait(unsigned long t){
  unsigned long currentTime = millis();
  if((currentTime - previusTimes) > t){
    previusTime = currentTime;
    ntp.update();
    ntpTime = ntp.getFormattedTime();
  }
}

/**
 * Set color led
 * */
void led(RGB colorname){
  analogWrite(pinR,colorname.r);
  analogWrite(pinG,colorname.g);
  analogWrite(pinB,colorname.b);
}

/**
 * function to do action requested by client
 * */
void handleAction(){

  if(hand){
  if(serverA.argName(0).equals("R")){
    analogWrite(pinR,map(serverA.arg(0).toInt(),0,255,0,15)); //enable PWM to red output
    color.r = map(serverA.arg(0).toInt(),0,255,0,15);
  }
  if(serverA.argName(1).equals("G")){
    analogWrite(pinG,map(serverA.arg(1).toInt(),0,255,0,15)); // enable PWM to green output
    color.g = map(serverA.arg(1).toInt(),0,255,0,15);
  }
  if(serverA.argName(2).equals("B")){
    analogWrite(pinB,map(serverA.arg(2).toInt(),0,255,0,15)); // enable PWM to blue output
    color.b = map(serverA.arg(2).toInt(),0,255,0,15);
  }
  String msg = "color{" + (String)color.r + (String)color.g + (String)color.b + "}";
  Serial.println(msg);
  }

  /// On Off Pump
  if(serverA.argName(0).equals("pump")){
    if(serverA.arg(0).equals("on")){
      //digitalWrite(pump,HIGH);
    if(serverA.argName(1).equals("prog1")){
      prog1 = serverA.arg(1);
    }
    if(serverA.argName(2).equals("prog2")){
      prog2 = serverA.arg(2);
    }
    }else if(serverA.arg(0).equals("off")){
      digitalWrite(pump,LOW);
      prog1 = "0";
      prog2 = "0";
    }

    Serial.print("Prog1: ");
    Serial.println(prog1);
    Serial.print("Prog2: ");
    Serial.println(prog2);
  }

  if(serverA.argName(0).equals("color")){
    if(serverA.arg(0).equals("auto")){
      hand = false;
      color = {15,0,0};
    }else if(serverA.arg(0).equals("handle")){
      hand = true;
    }
  }
  if(serverA.argName(1).equals("speed")){
    setTime = serverA.arg(1).toInt();
  }
  if(serverA.argName(0).equals("setPump")){
    if(serverA.arg(0).equals("set")){
      if(digitalRead(pump) == true){
        digitalWrite(pump,LOW);
        Serial.println("Pump off");
      }else{
        digitalWrite(pump,HIGH);
        Serial.println("Pump on");
      }
    }
  }
  Serial.print("speed: ");
  Serial.println((String)setTime);
  serverA.send(200);
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if(SPIFFS.begin()){
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = serverA.streamFile(file, contentType); // And send it to the client
    file.close(); 
    SPIFFS.end();                                      // Then close the file again
    return true;
  }
  SPIFFS.end();
  }
  Serial.println("\tFile Not Found");
  Serial.println(path);
  return false;                                         // If the file doesn't exist, return false
}

