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
 * @copyright Smart Home System Automação all copyrights reserved
 * 
 * */


/*********************
 * LIBRARIES
 * ******************/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
//#include <LittleFS.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFiManager.h>

/******************************************************
 * DEFINES
 ******************************************************/


/**************************************************************
 * GLOBAL VARIABLES
 **************************************************************/



const char* update_path = "/firmware"; /*!< Path for update*/
const char* filename = "/setting.json";
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
int btnPump = 4; /// define ldr sensor   
int prog = 0;  //Define program run
String page = ""; /*!< Store page HTML */
String agora = ""; /*!< Store ntp time */
int hoje = 0; /*!< Store day of week */
String ledStatus = ""; // Store status of led


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
  uint32_t ip = 1921680112;
  uint32_t gw = 19216801;
  uint32_t sn = 2552552550;
  int count = 0;
  bool pump;
  char led[16];
  bool programas [5][7];
  String horarios [5][2];
}config;

IPAddress _ip(config.ip);
IPAddress _gw(config.gw);
IPAddress _sn(config.sn);

//Config config;
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
void saveConfig(const char* filename, Config &conf);

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
 * @fn timeClock()
 * @brief  generates delay without locking the code
 * 
 *          set delay with millis() function
 *       
 * @return void
 * */
void timeClock();
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


/**
 * @fn handleActionPump()
 * @brief Function for handler pump
 *        Receiver command from client to start or stop pump
 * 
 * @return void
 *  
 * */
void handleActionPump();

/**
 * @fn handleActionLed()
 * @brief Function for handler led RGB color
 *        Turn color of led 
 * 
 * @return void
 * */
void handleActionLed();

int strToByte(String str);

/**
 * @fn handleUpdate();
 * @brief Function to update files in web
 * 
 * @return void
 * */
void handleUpdate();

void strTochar(String str, char * carc);


void handleProgramador();


/**
 * @class IPAddressParameter : public WiFiManagerParameter
 * @brief Classe conforme descrito na bibliote WiFimanage
 * 
 * @return IPAddress ip
 * */
class IPAddressParameter : public WiFiManagerParameter {
public:
    IPAddressParameter(const char *id, const char *placeholder, IPAddress address)
        : WiFiManagerParameter("") {
        init(id, placeholder, address.toString().c_str(), 16, "", WFM_LABEL_BEFORE);
    }

    bool getValue(IPAddress &ip) {
        return ip.fromString(WiFiManagerParameter::getValue());
    }
};


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
  
  //Parameters of setting
  WiFiManagerParameter custom_hostname("host","hostname",config.host,sizeof(config.host));
  WiFiManagerParameter custom_pass("pass","password",config.pass, sizeof(config.pass));
  WiFiManagerParameter custom_user("user","username",config.user, sizeof(config.user));
  IPAddressParameter   custom_ip("ip", "Static IP", _ip);
  IPAddressParameter   custom_gw("gw", "Gateway", _gw);
  IPAddressParameter   custom_sn("sn", "Sub-Mask", _sn);


  wifimanager.setSaveConfigCallback(saveCallbackConfig);

  wifimanager.addParameter(&custom_hostname);
  wifimanager.addParameter(&custom_user);
  wifimanager.addParameter(&custom_pass);
  wifimanager.addParameter(&custom_ip);
  wifimanager.addParameter(&custom_gw);
  wifimanager.addParameter(&custom_sn);
  _ip = config.ip;
  _gw = config.gw;
  _sn = config.sn;
  
  wifimanager.setSTAStaticIPConfig(_ip, _gw, _sn);

    String mac ="_";
    mac += WiFi.macAddress();
    mac = config.host + mac;
    if(!wifimanager.autoConnect(mac.c_str(), config.pass)){
      delay(3000);
      ESP.reset();
      delay(5000);
    }

    //Copy values and store in struct
    strcpy(config.host,custom_hostname.getValue());
    strcpy(config.pass,custom_pass.getValue());
    strcpy(config.user,custom_user.getValue());

    if(custom_ip.getValue(_ip)){
      config.ip = _ip;
    }

    if(custom_gw.getValue(_gw)){
      config.gw = _gw;
    }

    if(custom_sn.getValue(_sn)){
      config.sn = _sn;
    }

    saveConfig(filename,config);


  pinMode(pinR, OUTPUT); /* Set pin red as Output */
  pinMode(pinG, OUTPUT); /* Set pin green as Output */
  pinMode(pinB, OUTPUT); /* Set pin blue as Output */
  pinMode(pump, OUTPUT); /* Set pin pump as Output */
  pinMode(btnPump, INPUT); /* Set pin sensor sun as Input */

  digitalWrite(pump, LOW); /* Turn pump to off */
  digitalWrite(pinR,LOW); /* Turn off pin red */
  digitalWrite(pinG,LOW); /* Turn off pin green */
  digitalWrite(pinB,LOW); /* Turn off pin blue */

  analogWriteRange(255); // 8 bits resolution
  analogWriteFreq(1000); // 1khz frequency
  
  serverA.on("/", [](){
    //handleRoot(); 
        if (!handleFileRead(serverA.uri()))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  
  serverA.on("/update",handleUpdate); 

  serverA.on("/pump", handleActionPump);

  serverA.on("/led", handleActionLed);
  
  serverA.on("/programa", handleProgramador);

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
  
  serverA.handleClient();
  MDNS.update();
  timeClock();

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
 * Update online clock
 **/
void timeClock(){
  unsigned long currentTime = millis();
  if((currentTime - previusTimes) > 1000L){
    previusTimes = currentTime;
    ntp.update();
    agora = ntp.getFormattedTime();
    hoje = ntp.getDay();
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

void handleUpdate(){
    // Envia atualização de páginas
    if (!handleFileRead(filename))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found");
}

void handleProgramador(){
  
  if(serverA.argName(0).equals("prog")){
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, serverA.arg(0));
    if(error) Serial.println("Failed to read file, using default config");
    copyArray(doc["pgr"], config.programas);
  }
  if(serverA.argName(1).equals("hora")){
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, serverA.arg(1));
    if(error) Serial.println("Failed to read file, using default config");
    copyArray(doc["hr"], config.horarios);
  }
  if(serverA.argName(2).equals("count")){
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, serverA.arg(2));
    if(error) Serial.println("Failed to read file, using default config");
    config.count = doc["count"];
  }
  saveConfig(filename, config);
  serverA.send(200, "text/plain","");
}

/**
 * function to do action requested by client
 * */
void handleActionPump(){
    //ativar bomba
    if(serverA.argName(0).equals("setPump")){
      if(serverA.arg(0).equals("on")){
        digitalWrite(pump, HIGH);
        config.pump = true;
      }else if(serverA.arg(0).equals("off")){
        digitalWrite(pump, LOW);
        config.pump = false;
      }
    }
    saveConfig(filename, config);
  }

void handleActionLed(){
  //ativar led
  if(serverA.argName(0).equals("color")){
    color.r = strToByte(serverA.arg(0).substring(1,3)); // get red color 
    color.g = strToByte(serverA.arg(0).substring(3,5)); // get green color 
    color.b = strToByte(serverA.arg(0).substring(5));   // get blue color
    led(color);
    
  }else if(serverA.argName(0).equals("estado")){
    ledStatus = serverA.arg(0);
    strTochar(ledStatus, config.led);
    saveConfig(filename, config);
  }

}

void strTochar(String str, char * carc){
  for(int i = 0; i < str.length(); i++){
    carc[i] = str[i];
  }
}

//Convert string to byte
int strToByte(String str){
  byte p1 = 0;
  byte p2 = 0;
  if((str[0] >= 48) && (str[0] <= 57)){
    p1 = (str[0] - 48) * 16; 
  }else if((str[0] >= 97) && (str[0] <= 102)){
    p1 = (str[0] - 87) * 16;
  }

  if((str[1] >= 48) && (str[1] <= 57)){
    p2 = str[1] - 48;
  }else if((str[1] >= 97) && (str[1] <= 102)){
    p2 = str[1] - 87;
  }
  return (p1 + p2);
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".json")) return "application/json";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
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
  return false;                                         // If the file doesn't exist, return false
}

void loadConfigurator(const char* filename, Config &config){

  if(SPIFFS.begin()){
    File file;
    if(SPIFFS.exists(filename)){
      file = SPIFFS.open(filename, "r");
    }else{
      file = SPIFFS.open(filename,"w");
    }
    
    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc,file);
    if(error) Serial.println("Failed to read file, using default config");
    strlcpy(config.host,doc["hostname"]|"aqua",sizeof(config.host));
    strlcpy(config.pass,doc["pass"]|"admin123",sizeof(config.pass));
    strlcpy(config.user,doc["user"]|"admin",sizeof(config.user));
    config.count = doc["count"];
    config.ip = doc["ip"];
    IPAddress _ip(config.ip);
    config.gw = doc["gw"];
    config.sn = doc["sn"];
    config.pump = doc["Pump"];
    strlcpy(config.led, doc["Led"]|"000", sizeof(config.led));
    copyArray(doc["pgr"],config.programas);
    copyArray(doc["hr"], config.horarios);
    file.close();
    SPIFFS.end();
  }

}

void saveConfig(const char* filename, Config &conf){

  Config dados = conf;
  
  if(SPIFFS.begin()){
    if(SPIFFS.exists(filename)){
      SPIFFS.remove(filename);
    }
    File file = SPIFFS.open(filename,"w");
    if(!file){
      SPIFFS.end();
      return;
    }
    Serial.println("Criando DOC..");
    StaticJsonDocument<4096> doc;
    doc["hostname"] = dados.host;
    doc["pass"] = dados.pass;
    doc["user"] = dados.user;
    doc["ip"] = dados.ip;
    doc["gw"] = dados.gw;
    doc["sn"] = dados.sn;
    doc["Pump"] = dados.pump;
    doc["Led"] = dados.led;
    doc["count"] = dados.count;
    Serial.println("Anexando pgr");
    copyArray(dados.programas, doc["pgr"]);
    Serial.println("Anexando hr");
    copyArray(dados.horarios, doc["hr"]);
    Serial.println("Serializando...");
    if(serializeJson(doc,file) == 0){
      Serial.println("Error!");
    }
    Serial.println("Concluido!");
    file.close();
    SPIFFS.end();
  }
}

void saveCallbackConfig(){
  shouldSave = true;
}