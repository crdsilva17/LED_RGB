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

 DynamicJsonDocument doc(8192);


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
NTPClient ntp(ntpUDP,"a.st1.ntp.br", -3 * 3600, 60000);

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
  IPAddress ip = {192,168,0,112};
  IPAddress gw = {192,168,0,1};
  IPAddress sn = {255,255,255,0};
  RGB corLed = {0,0,0};
  int count = 0;
  bool pump = false;
  bool dhcp = true;
  bool sun = false;
  char led[16];
  bool programas [5][7];
  String horarios [5][2];
  int speed = 10;
}config;




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

void handleIp();

void sequenciaLed();
void aleatorioLed();
void dimmerLed();

void handleReset();


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
 // Serial.begin(9600);

  loadConfigurator(filename, config);
  
  //Parameters of setting
  WiFiManagerParameter custom_hostname("host","hostname",config.host,sizeof(config.host));
  WiFiManagerParameter custom_pass("pass","password",config.pass, sizeof(config.pass));
  WiFiManagerParameter custom_user("user","username",config.user, sizeof(config.user));
  WiFiManagerParameter custom_ip("ip", "Static IP", config.ip.toString().c_str(), 16);
  WiFiManagerParameter custom_gw("gw", "Gateway", config.gw.toString().c_str(), 16);
  WiFiManagerParameter custom_sn("sn", "Sub-Mask", config.sn.toString().c_str(), 16);


  wifimanager.setSaveConfigCallback(saveCallbackConfig);

  wifimanager.addParameter(&custom_hostname);
  wifimanager.addParameter(&custom_user);
  wifimanager.addParameter(&custom_pass);
  wifimanager.addParameter(&custom_ip);
  wifimanager.addParameter(&custom_gw);
  wifimanager.addParameter(&custom_sn);
  
  if(config.dhcp)
    wifimanager.setSTAStaticIPConfig(config.ip, config.gw, config.sn);

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

    config.ip.fromString(custom_ip.getValue());
    config.gw.fromString(custom_gw.getValue());
    config.sn.fromString(custom_sn.getValue());

    saveConfig(filename,config);


  pinMode(pinR, OUTPUT); /* Set pin red as Output */
  pinMode(pinG, OUTPUT); /* Set pin green as Output */
  pinMode(pinB, OUTPUT); /* Set pin blue as Output */
  pinMode(pump, OUTPUT); /* Set pin pump as Output */
  pinMode(btnPump, INPUT); /* Set pin sensor sun as Input */

  digitalWrite(pump, HIGH); /* Turn pump to off */
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

  serverA.on("/meuIp", handleIp);
  
  serverA.on("/programa", handleProgramador);

  serverA.on("/reset", handleReset);

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

  String str(config.led);
  float h = agora.substring(0, agora.indexOf(":")).toFloat() + (agora.substring(agora.indexOf(":")+1,agora.indexOf(":",agora.indexOf(":")+1)).toFloat()/60);
  
  if((config.sun == false) || (h >= 17.5)||(h <= 5.99)){
  if(str.indexOf("sequencia") > -1){
    sequenciaLed();
  }else if(str.indexOf("aleatorio") > -1){
    aleatorioLed();
  }else if(str.indexOf("dimmer") > -1){
    dimmerLed();
  }
  }else{
    if(!str.indexOf("manual")> -1){
      config.corLed = {0,0,0};
      led(config.corLed);
    }
  }

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
  serverA.handleClient();
  
  //Verificar Programa para controle automático da bomba
  if(config.count > 0){ //caso exista programa
   bool oldPump = config.pump;
   float hp, hp1, ha;
   float mp, mp1, ma;
   String aux;
    for(int i=0; i<config.count; i++){ //cria loop para ler todos os programas
      if(config.programas[i][hoje]){   // Compara se dia atual esta habilitado
        hp = config.horarios[i][0].substring(0, config.horarios[i][0].indexOf(":")).toFloat();
        hp1 = config.horarios[i][1].substring(0, config.horarios[i][1].indexOf(":")).toFloat();
        mp = config.horarios[i][0].substring(config.horarios[i][0].indexOf(":") + 1).toFloat();
        mp1 = config.horarios[i][1].substring(config.horarios[i][1].indexOf(":") + 1).toFloat();
        hp += mp/60;
        hp1 += mp1/60;
        ha = agora.substring(0, agora.indexOf(":")).toFloat();
        ma = agora.substring(agora.indexOf(":")+1,agora.indexOf(":",agora.indexOf(":")+1)).toFloat();
        ha += ma/60;
        if(hp <= ha && hp1> ha){ // Verifica se horário atual esta no intervalo de bomba ligada
          digitalWrite(pump, LOW);   // Liga bomba
          config.pump = true;        // Atualiza variavel que indica estado da bomba
          pumpAuto = true;           // indica que a bomba opera em modo automático
        }else if(hp1<= ha && pumpAuto){  // verifica se esta na hora de desligar bomba
          digitalWrite(pump,HIGH); // Desliga bomba;
          config.pump = false;   // Atualiza variavel que indica estado da bomba.
          pumpAuto = false;     //Indica que a bomba deixou o modo automático.
        }
      }
    }
    if(oldPump != config.pump){ //Salva apenas se valor mudar
    //atualiza arquivo Json
    saveConfig(filename, config); 
    }
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

void sequenciaLed(){
  config.corLed = {255, 0, 0};
  int i = 0;
  for(i = 0; i <= 255; i++){
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 255; i>= 0; i--){
    config.corLed.r = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 0; i<=255; i++){
    config.corLed.g = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 255; i>=0; i--){
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 0; i <= 255; i++){
    config.corLed.r = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 255; i>=0; i--){
    config.corLed.g = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
}

void aleatorioLed(){
  randomSeed(random(256));
  config.corLed.r = random(256);
  config.corLed.g = random(256);
  config.corLed.b = random(256);
  led(config.corLed);
  delay(config.speed);
}

void dimmerLed(){
  int i = 0;
  for(i = 0; i<=255; i++){
    config.corLed.r = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=0; i<=255; i++){
    config.corLed.g = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=255; i>=0; i--){
    config.corLed.r = i;
    config.corLed.g = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=0; i<=255; i++){
    config.corLed.r = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 0; i<=255; i++){
    config.corLed.g = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=255; i>=0; i--){
    config.corLed.r = i;
    config.corLed.g = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 0; i<=255; i++){
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=0; i<=255; i++){
    config.corLed.r = i;
    config.corLed.g = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=255; i>=0; i--){
    config.corLed.r = i;
    config.corLed.g = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=0; i<=255; i++){
    config.corLed.b = i;
    config.corLed.g = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 0; i<=255; i++){
    config.corLed.r = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=255; i>=0; i--){
    config.corLed.r = i;
    config.corLed.g = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 0; i<=255; i++){
    config.corLed.g = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=0; i<=255; i++){
    config.corLed.b = i;
    config.corLed.r = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=255; i>=0; i--){
    config.corLed.r = i;
    config.corLed.g = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=0; i<=255; i++){
    config.corLed.g = i;
    config.corLed.r = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i = 0; i<=255; i++){
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }
  for(i=255; i>=0; i--){
    config.corLed.r = i;
    config.corLed.g = i;
    config.corLed.b = i;
    led(config.corLed);
    timeClock();
    delay(config.speed);
  }

}

void handleUpdate(){
    // Envia atualização de páginas
    if (!handleFileRead(filename))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found");
}

void handleIp(){
  if(serverA.argName(0).equals("meuip")){
    StaticJsonDocument<1024> doc1;
    deserializeJson(doc1,serverA.arg(0));
    for(int i = 0; i<4; i++){
    config.ip[i] = doc1["ip"][i];
    config.gw[i] = doc1["gw"][i];
    config.sn[i] = doc1["sn"][i];
    }
    config.dhcp = doc1["dhcp"];
    saveConfig(filename, config);
    serverA.send(200, "text/plain","");
  }
}

void handleProgramador(){
  
  if(serverA.argName(0).equals("prog")){
    ///StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, serverA.arg(0));
    if(error) Serial.println("Failed to read file, using default config");
    copyArray(doc["pgr"], config.programas);
  }
  if(serverA.argName(1).equals("hora")){
    ///StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, serverA.arg(1));
    if(error) Serial.println("Failed to read file, using default config");
    copyArray(doc["hr"], config.horarios);
  }
  if(serverA.argName(2).equals("count")){
    ///StaticJsonDocument<1024> doc;
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
        digitalWrite(pump, LOW);
        config.pump = true;
        pumpAuto = false;
      }else if(serverA.arg(0).equals("off")){
        digitalWrite(pump, HIGH);
        config.pump = false;
      }
    }
    serverA.send(200,"text/plain","");
    saveConfig(filename, config);
  }

void handleActionLed(){
  //ativar led
  if(serverA.argName(0).equals("cor")){
    config.corLed.r = strToByte(serverA.arg(0).substring(1,3)); // get red color 
    config.corLed.g = strToByte(serverA.arg(0).substring(3,5)); // get green color 
    config.corLed.b = strToByte(serverA.arg(0).substring(5));   // get blue color
    led(config.corLed);
  }else if(serverA.argName(0).equals("estado")){
    strTochar(serverA.arg(0), config.led);
    config.speed = serverA.arg(1).toInt();
  }else if(serverA.argName(0).equals("sun")){
    if(serverA.arg(0).equals("0")){
      config.sun = false;
    }else if(serverA.arg(0).equals("1")){
      config.sn = true;
    }
  } 
  serverA.send(200, "text/plain","");
  saveConfig(filename, config);

}

//Trata as ações de reset do módulo
void handleReset(){
  if(serverA.argName(0).equals("reset")){ // Verifica nome de dados recebidos
    if(serverA.arg(0).equals("again")){ // Aqui reinicia módulo mantendo dados
      serverA.send(200, "text/plain","Reset Device...");
      delay(1000);
      ESP.reset();
      delay(1000);
    }else if(serverA.arg(0).equals("default")){ // Aqui reinicia módulo ao padrão de fábrica
      serverA.send(200,"text/plain", "Default Reset....");
      delay(1000);
      wifimanager.resetSettings();
      if(SPIFFS.remove(filename)) Serial.println("Arquivos Deletados");
      ESP.reset();
      delay(1000);
    }
  }
}

///Converte String para Caracter
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
  }else{
    path += ".gz";
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

///Carrega configurações previamente salvas
void loadConfigurator(const char* filename, Config &config){

  if(SPIFFS.begin()){ ///inicializa sistema de arquivos
    File file;
    if(SPIFFS.exists(filename)){ // Verifica existencia de arquivo
      file = SPIFFS.open(filename, "r");  //Existindo abre em modo de leitura
    }else{
      file = SPIFFS.open(filename,"w"); /// caso contrário cria arquivo para escrita de valores.
    }
    DeserializationError error = deserializeJson(doc,file); // formata em modelo Json
    if(error) Serial.println("Failed to read file, using default config");
    strlcpy(config.host,doc["hostname"]|"aqua",sizeof(config.host)); //Grava dados de hostname,
    strlcpy(config.pass,doc["pass"]|"admin123",sizeof(config.pass)); // Senha de acesso para atualização,
    strlcpy(config.user,doc["user"]|"admin",sizeof(config.user));    // nome de usuário,
    strlcpy(config.led, doc["Led"]|"manual", sizeof(config.led));   // estado do Led,
    config.count = doc["count"];  ///Quantidades de programas,
    for(int i = 0; i<4; i++){     /// IP, Gateway, Sub-Rede,
    config.ip[i] = doc["ip"][i];
    config.gw[i] = doc["gw"][i];
    config.sn[i] = doc["sn"][i];
    }
    config.sun = doc["sun"];
    config.speed = doc["delay"]|10; //atualiza velocidade de transição na cor dos leds
    config.dhcp = doc["dhcp"]|true; // dados de dhcp manual ou automático,
    config.pump = doc["Pump"];      // estado da bomba (on ou Off),
    config.corLed.r = doc["cor_r"];  ///intenciadade da cor vermelha no led,
    config.corLed.g = doc["cor_g"]; //intencidade da cor verde no led,
    config.corLed.b = doc["cor_b"]; //intencidade da cor azul no led,
    copyArray(doc["pgr"],config.programas); // Programações da bomba e 
    copyArray(doc["hr"], config.horarios);   // horarios de ativação da bomba.
    file.close();
    SPIFFS.end();
  }

}


//Salva dados de configuração do módulo
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
   /// StaticJsonDocument<2048> doc;
    doc["hostname"] = dados.host;
    doc["pass"] = dados.pass;
    doc["user"] = dados.user;
    for(int i = 0; i<4; i++){
     doc["ip"][i] = dados.ip[i];
     doc["gw"][i] = dados.gw[i];
     doc["sn"][i] = dados.sn[i];
    }
    doc["delay"] = dados.speed;
    doc["dhcp"] = dados.dhcp;
    doc["Pump"] = dados.pump;
    doc["cor_r"] = dados.corLed.r;
    doc["cor_g"] = dados.corLed.g;
    doc["cor_b"] = dados.corLed.b;
    doc["Led"] = dados.led;
    doc["count"] = dados.count;
    doc["sun"] = dados.sun;
    copyArray(dados.programas, doc["pgr"]);
    copyArray(dados.horarios, doc["hr"]);
    if(serializeJson(doc,file) == 0){
      Serial.println("Error!");
    }
    file.close();
    SPIFFS.end();
  }
}

void saveCallbackConfig(){
  shouldSave = true;
}