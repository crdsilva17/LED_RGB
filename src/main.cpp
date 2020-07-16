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
#include <WiFiManager.h>
#include <ArduinoJson.h>

/******************************************************
 * DEFINES
 ******************************************************/


/**************************************************************
 * GLOBAL VARIABLES
 **************************************************************/



const char* update_path = "/firmware"; /*!< Path for update*/
const char* filename = "/config.txt";
bool hand = true; /*!< handle manual or automatic action*/
bool pumpAuto = false; /*!< store status pump auto*/
bool shouldSave = false;
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
String prog1 = "00:00:00";/*!< store time on pump*/
String prog2 = "00:00:00";/*!< store time off pump*/


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
 * @class serverA(port)
 * @brief WebServer Class
 * 
 * This create a object for handle request response of client
 * 
 * @param port - Web port
 * 
 * @see other classes
 *      - httpUpdater
 *      - RGB
 **/
  ESP8266WebServer serverA(80); 

/**
 * @class wifimanager
 * @brief WiFiManager Class
 * 
 * */
  WiFiManager wifimanager;

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
  
/**
 * @struct Config 
 * @brief Struct for setting
 * 
 * Generates struct for setting object
 * 
 * @see other classes
 *      - serverA
 *      - httpUpdater
 *      - RGB
 */
struct Config{
  char user[20]; /*!< username for login*/
  char pass[16]; /*!< paasword for access*/
  char host[20]; /*!< hostname for local access*/
};

Config config;
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
 * @fn loadConfigurator(const char* filename, Config &config)
 * @brief read config file
 * 
 * @param filename - const char*
 * @param config - Config
 * @return void
 * */
void loadConfigurator(const char* filename, Config &config);

/**
 * @fn saveCallbackConfig()
 * @brief set flag save config
 * 
 * 
 * */
void saveCallbackConfig();

/**
 * @fn saveConfig(const char* filename, Config &config)
 * @brief read config file
 * 
 * @param filename - const char*
 * @param config - Config
 * @return void
 * */
void saveConfig(const char* filename, Config &config);

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
void wait1(unsigned long t);
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

  loadConfigurator(filename, config);
  Serial.println("Prepair parameters");
  
  //Parameters of setting
  WiFiManagerParameter custom_hostname("host","hostname",config.host,sizeof(config.host));
  WiFiManagerParameter custom_pass("pass","password",config.pass, sizeof(config.pass));
  WiFiManagerParameter custom_user("user","username",config.user, sizeof(config.user));

  wifimanager.setSaveConfigCallback(saveCallbackConfig);

  wifimanager.addParameter(&custom_hostname);
  wifimanager.addParameter(&custom_user);
  wifimanager.addParameter(&custom_pass);

    String mac ="_";
    mac += WiFi.macAddress();
    mac = config.host + mac;
    if(!wifimanager.autoConnect(mac.c_str() ,config.pass)){
      Serial.println("Failed to connect");
      delay(3000);
      ESP.reset();
      delay(5000);
    }

    //Copy values and store in struct
    strcpy(config.host,custom_hostname.getValue());
    strcpy(config.pass,custom_pass.getValue());
    strcpy(config.user,custom_user.getValue());

    saveConfig(filename,config);


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

 /*
  WiFi.mode(WIFI_STA); /* Set device as Station mode */
 /* WiFi.begin(config.ssid,config.passwd); /* Try connect to network */
  /*Serial.print("Connecting ");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }*/
  Serial.println("Connected.");
  Serial.println(WiFi.localIP().toString());
  
  serverA.on("/", [](){
    //handleRoot(); 
        if (!handleFileRead(serverA.uri()))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  
  serverA.on("/update",handleAction); 

  serverA.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(serverA.uri()))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  
  MDNS.begin(config.host);
  httpUpdater.setup(&serverA, update_path, config.user, config.pass);
  serverA.begin();
  ntp.begin();
  MDNS.addService("http", "tcp", 80);

}



void loop() {
  
  if(hand != true){
    if(digitalRead(sunLight)){
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
    }else{
      color = {0,0,0};
      led(color);
    }
    led(color);
    }
  if(pumpAuto == true){
  if((prog1 == ntpTime)&&(digitalRead(pump) == LOW)){
    Serial.println("Pump on");
    digitalWrite(pump, HIGH);
  }
  if((prog2 == ntpTime)&&(digitalRead(pump) == HIGH)){
    Serial.println("Pump off");
    digitalWrite(pump, LOW);
  }
  }
  
  serverA.handleClient();
  MDNS.update();
  wait1(1000L);

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
void wait1(unsigned long t){
  unsigned long currentTime = millis();
  if((currentTime - previusTimes) > t){
    previusTimes = currentTime;
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

  if(serverA.argName(0).equals("update")){ //update value for client
    String json;
    json.reserve(128);
    json = "{\"led\":";
    if(hand){
    json += "\"1\"";
    }else{
    json += "\"0\"";
    }
    json += ", \"pump\":\"";
    json += pumpAuto;
    json += "\", \"red\":\"";
    json += map(color.r,0,15,0,255);
    json += "\", \"green\":\"";
    json += map(color.g,0,15,0,255);
    json += "\", \"blue\":\"";
    json += map(color.b,0,15,0,255);
    json += "\", \"prog1\":\"";
    json += prog1;
    json += "\", \"prog2\":\"";
    json += prog2;
    json += "\", \"range\":\"";
    json += setTime;
    json += "\"}";

    serverA.send(200,"application/json", json);
    
  }

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
      pumpAuto = true;
    if(serverA.argName(1).equals("prog1")){
      prog1 = serverA.arg(1);
    }
    if(serverA.argName(2).equals("prog2")){
      prog2 = serverA.arg(2);
    }
    }else if(serverA.arg(0).equals("off")){
      digitalWrite(pump,LOW);
      pumpAuto = false;
    if(serverA.argName(1).equals("prog1")){
      prog1 = serverA.arg(1);
    }
    if(serverA.argName(2).equals("prog2")){
      prog2 = serverA.arg(2);
    }
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
      color = {0,0,0};
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

void loadConfigurator(const char* filename, Config &config){

  if(SPIFFS.begin()){
    Serial.println("mount File system.....");
    File file;
    if(SPIFFS.exists(filename)){
      Serial.println("file found!!!");
      file = SPIFFS.open(filename, "r");
      Serial.println("readinng file....");
    }else{
      Serial.println("file not found!!  Create");
      file = SPIFFS.open(filename,"w");
    }
    
    StaticJsonDocument<256> doc;
    Serial.println("doc create");
    DeserializationError error = deserializeJson(doc,file);
    Serial.println("deserialization");
    if(error) Serial.println("Failed to read file, using default config");
    strlcpy(config.host,doc["hostname"]|"aqua",sizeof(config.host));
    strlcpy(config.pass,doc["pass"]|"senha123",sizeof(config.pass));
    strlcpy(config.user,doc["user"]|"root",sizeof(config.user));
    file.close();
    Serial.println("close file");
    SPIFFS.end();
    Serial.println("end System file");
  }

}

void saveConfig(const char* filename, Config &config){
  
  if(SPIFFS.begin()){
    Serial.println("mount File system....");
    if(SPIFFS.exists(filename)){
      SPIFFS.remove(filename);
    }
    Serial.println("Create file");
    File file = SPIFFS.open(filename,"w");
    if(!file){
      Serial.println("Failed to create file");
      SPIFFS.end();
      return;
    }
    StaticJsonDocument<256> doc;
    doc["hostname"] = config.host;
    doc["pass"] = config.pass;
    doc["user"] = config.user;
    if(serializeJson(doc,file) == 0){
      Serial.println("Failed to write file");
    }
    file.close();
    SPIFFS.end();
  }
}

void saveCallbackConfig(){
  Serial.println("set save flag true");
  shouldSave = true;
}