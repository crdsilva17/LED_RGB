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

#define SHOW_TIME_PERIOD 1000
#define NTP_TIMEOUT 1500

/**************************************************************
 * GLOBAL VARIABLES
 **************************************************************/
int8_t timeZone               = -3;
int8_t minutesTimeZone        = 0;
int8_t ntpPort                = 123;
const PROGMEM char *ntpServer = "pool.ntp.org";
const char* update_path       = "/firmware"; /*!< Path for update*/
const char* file1             = "/home.txt";
const char* file2             = "/led.txt";
const char* file3             = "/prog.txt";
const char* file4             = "/conf.txt";
bool pumpAuto                 = false; /*!< store status pump auto*/
bool shouldSave               = false;
unsigned long previusTime     = 0; /*!< receive previus time for trigger delay */
unsigned long previusTimes    = 0; /*!< receive previus time for trigger delay ntp */
File uploadFile; /*!< Store data for SPIFFS readed*/
int pinR                      = 12; /*!< Set pin red as number 12 */
int pinG                      = 13; /*!< Set pin green as number 13 */
int pinB                      = 14; /*!< Set pin blue as number 14 */
int pump                      = 5; ///< define pin of pump
int btnPump                   = 4; /// define ldr sensor   
int prog                      = 0;  //Define program run
String page                   = ""; /*!< Store page HTML */
String agora                  = "00:00"; /*!< Store ntp time */
short seg                     = 0;
int hoje                      = 0; /*!< Store day of week */

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
  char user[20]           = "admin"; /*!< username for login*/
  char pass[16]           = "admin123"; /*!< paasword for access*/
  char host[20]           = "aqua"; /*!< hostname for local access*/
  String ntpServ          ="pool.ntp.org"; /*!< Server ntp */
  int port                = 123; /*!< Port value ntp>*/
  int8_t zone             = -3; /*!< timeZone ntp*/
  RGB corLed              = {0,0,0}; /*!< Struct RGB color*/
  int count               = 0;   /*!< store number of timer */
  bool pump               = false; /*!< state pump pool*/
  bool sun                = false;
  char led[16]            = "manual";
  int programas [5][7]    = {{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
  String horarios [5][2]  = {{"00:00", "00:00"},{"00:00", "00:00"},{"00:00", "00:00"},{"00:00", "00:00"},{"00:00", "00:00"}};
  unsigned int speed      = 500;
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
 * @fn loadConfigurator(const char* filenome, Config &config)
 * @brief read config file
 * 
 * @param filenome - const char*
 * @param config - Config
 * @return void
 * */
void loadConfigurator(const char* filenome, Config &config);

/**
 * @fn saveCallbackConfig()
 * @brief set flag save config
 * 
 * 
 * */
void saveCallbackConfig();

/**
 * @fn saveConfig(const char* filenome, Config &config)
 * @brief read config file
 * 
 * @param filenome - const char*
 * @param config - Config
 * @return void
 * */
void saveConfig(const char* filenome, Config &conf);

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
 * @fn getContentType(String filenome)
 * @brief Convert the file extension to the MIME type
 * 
 *        set of the request type for client 
 *        .html
 *        .css
 *        .js
 *        .ico
 * 
 * @param filenome - String
 * @return String 
 * */
String getContentType(String filenome); // convert the file extension to the MIME type

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

/**
 * @brief Convert string to Byte
 * 
 * @param str 
 * @return int 
 */
int strToByte(String str);

/**
 * @fn handleUpdate();
 * @brief Function to update files in web
 * 
 * @return void
 * */
void handleUpdate();


/**
 * @fn strToChar(String str, char *carc)
 * 
 * @brief Convert string to character
 * 
 * @param str 
 * @param carc 
 */
void strTochar(String str, char * carc);

/**
 * @brief Handler programmable timer  
 * 
 */
void handleProgramador();


/**
 * @brief handler IP Address
 * 
 */
//void handleIp();

/**
 * @brief handler reset of device
 * 
 */
void handleReset();

 /**
 * @class ntp
 * @brief create client ntp
 * 
 * */
 NTPClient ntp(ntpUDP, config.ntpServ.c_str(), config.zone * 3600);



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
  
  Serial.begin(9600);

  delay(3000);
  loadConfigurator(file1, config);
  delay(1000);
  loadConfigurator(file2, config);
  delay(1000);
  loadConfigurator(file3, config);
  delay(1000);
  loadConfigurator(file4, config);
  delay(3000); //Aguarda 3 segundo
 


  //Parameters of setting
  WiFiManagerParameter custom_hostname("host","hostname",config.host,sizeof(config.host));
  //WiFiManagerParameter custom_pass("pass","password",config.pass, sizeof(config.pass));
  //WiFiManagerParameter custom_user("user","username",config.user, sizeof(config.user));
  delay(1000);
  wifimanager.setSaveConfigCallback(saveCallbackConfig);
  delay(3000);
  wifimanager.addParameter(&custom_hostname);
  //wifimanager.addParameter(&custom_user);
  //wifimanager.addParameter(&custom_pass);

  delay(3000);

  String mac ="_";
  mac += WiFi.macAddress();
  mac = config.host + mac;
  delay(3000);
  if(!wifimanager.autoConnect(mac.c_str(), config.pass)){
    delay(3000);
    ESP.reset();
    delay(5000);
  }



  //Copy values and store in struct
  strcpy(config.host,custom_hostname.getValue());
  //strcpy(config.pass,custom_pass.getValue());
  //strcpy(config.user,custom_user.getValue());


  saveConfig(file1,config);
  saveConfig(file2,config);
  saveConfig(file3,config);
  saveConfig(file4,config);

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
  analogWriteFreq(5000); // 5khz frequency
  
  serverA.on("/", [](){
    //handleRoot(); 
    if (!handleFileRead(serverA.uri()))                  // send it if it exists
    serverA.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  
  serverA.on("/update",handleUpdate); 

  serverA.on("/pump", handleActionPump);

  serverA.on("/led", handleActionLed);
  
  serverA.on("/programa", handleProgramador);

  serverA.on("/updateClock",[](){
    agora = ntp.getFormattedTime();
    serverA.send(200, "text/plain", agora);
  });

  serverA.on("/clock",[](){
    if(serverA.argName(0).equals("ntpServer"))
        config.ntpServ = serverA.arg(0);

    if(serverA.argName(1).equals("port"))
        config.port = serverA.arg(1).toInt();

    if(serverA.argName(2).equals("ntpZone"))
        config.zone = serverA.arg(2).toInt();
    

    if(serverA.argName(3).equals("hora")){
        agora = serverA.arg(3);
    }
    saveConfig(file4, config);
    serverA.send(200, "text/plain", "");

  });

  serverA.on("/reset", handleReset);

  serverA.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(serverA.uri()))                  // send it if it exists
        serverA.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  WiFi.mode(WIFI_STA);
  delay(1000);
  MDNS.begin(config.host);
  httpUpdater.setup(&serverA, update_path, config.user, config.pass);
  serverA.begin();
  MDNS.addService("http", "tcp", 80);
  ntp.begin();
  delay(1000);
}



void loop() {
  float hp, hp1, ha;
  float mp, mp1, ma;
  
  serverA.handleClient();

  MDNS.update();

  if(ntp.update()){
    timeClock();
  }

  ha = agora.substring(0, agora.indexOf(":")).toFloat();
  ma = agora.substring(agora.indexOf(":")+1,agora.indexOf(":",agora.indexOf(":")+1)).toFloat();
  ha += ma/60;

 //Verificar Programa para controle automático da bomba
  if(config.count > 0){ //caso exista programa
   bool oldPump = config.pump;
   String aux;
    for(int i=0; i<config.count; i++){ //cria loop para ler todos os programas
      if(config.programas[i][hoje]){   // Compara se dia atual esta habilitado
        hp = config.horarios[i][0].substring(0, config.horarios[i][0].indexOf(":")).toFloat();
        hp1 = config.horarios[i][1].substring(0, config.horarios[i][1].indexOf(":")).toFloat();
        mp = config.horarios[i][0].substring(config.horarios[i][0].indexOf(":") + 1).toFloat();
        mp1 = config.horarios[i][1].substring(config.horarios[i][1].indexOf(":") + 1).toFloat();
        hp += mp/60;
        hp1 += mp1/60;

        if(hp <= ha && hp1> ha && !config.pump){ // Verifica se horário atual esta no intervalo de bomba ligada
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
      saveConfig(file1, config);
       saveConfig(file2, config);
    }
  }

  if((config.sun && (ha > 18.5 || ha < 6.0))|| !config.sun){
  if(String(config.led).indexOf("sequencia") != -1 && wait()){
    if(config.corLed.r == 0 && config.corLed.g == 0 && config.corLed.b == 0){
      config.corLed = {255, 0, 0};
    }else if(config.corLed.r == 255 && config.corLed.g == 0 && config.corLed.b == 0){
      config.corLed = {255, 0, 255};
    }else if(config.corLed.r == 255 && config.corLed.g == 0 && config.corLed.b == 255){
      config.corLed = {0, 0, 255};
    }else if(config.corLed.r == 0 && config.corLed.g == 0 && config.corLed.b == 255){
      config.corLed = {0, 255, 255};
    }else if(config.corLed.r == 0 && config.corLed.g == 255 && config.corLed.b == 255){
      config.corLed = {0, 255, 0};
    }else if(config.corLed.r == 0 && config.corLed.g == 255 && config.corLed.b == 0){
      config.corLed = {255, 255, 0};
    }else if(config.corLed.r == 255 && config.corLed.g == 255 && config.corLed.b == 0){
      config.corLed = {255, 0, 0};
    }
    led(config.corLed);
  }
  }else{
    if(String(config.led).indexOf("sequencia") != -1)
      led({0,0,0});
  }

}
/**
 * Wait for set time
 **/
bool wait(){
  unsigned long currentTime = millis();
  if((currentTime - previusTime) >= config.speed){
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
  if((currentTime - previusTimes) > SHOW_TIME_PERIOD){
    previusTimes = currentTime;

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
    if (!handleFileRead(page))                  // send it if it exists
      serverA.send(404, "text/plain", "404: Not Found");
}

void handleProgramador(){
  
  if(serverA.argName(0).equals("prog")){
    DynamicJsonDocument doc1(1024);
    DeserializationError error = deserializeJson(doc1, serverA.arg(0));
    if(error) Serial.println("Failed to read file, using default config");
    copyArray(doc1["pgr"], config.programas);
  }
  if(serverA.argName(1).equals("hora")){
    DynamicJsonDocument doc2(1024);
    DeserializationError error = deserializeJson(doc2, serverA.arg(1));
    if(error) Serial.println("Failed to read file, using default config");
    copyArray(doc2["hr"], config.horarios);
  }
  if(serverA.argName(2).equals("count")){
    StaticJsonDocument<256> doc3;
    DeserializationError error = deserializeJson(doc3, serverA.arg(2));
    if(error) Serial.println("Failed to read file, using default config");
    config.count = doc3["count"];
  }
  saveConfig(file3, config);
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
    saveConfig(file1, config);
     saveConfig(file2, config);
    serverA.send(200,"text/plain","");
  }

void handleActionLed(){
  //ativar led
  if(serverA.argName(0).equals("cor")){
    config.corLed.r = strToByte(serverA.arg(0).substring(1,3)); // get red color 
    config.corLed.g = strToByte(serverA.arg(0).substring(3,5)); // get green color 
    config.corLed.b = strToByte(serverA.arg(0).substring(5));   // get blue color
    led(config.corLed);
  }else if(serverA.argName(0).equals("estado")){
    config.corLed = {0,0,0};
    strTochar(serverA.arg(0), config.led);
    config.speed = serverA.arg(1).toInt();
    if(serverA.arg(2).equals("0")){
      config.sun = false;
    }else if(serverA.arg(2).equals("1")){
      config.sun = true;
    }
  
  } 
  
  saveConfig(file2, config);
   saveConfig(file1, config);
  serverA.send(200, "text/plain","");

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
      delay(3000);
      ESP.eraseConfig();
      delay(3000);

      if(SPIFFS.remove(file1) && SPIFFS.remove(file2) && SPIFFS.remove(file3) && SPIFFS.remove(file4)) 
      Serial.println("Arquivos Deletados");

      delay(3000);
      ESP.reset();
      delay(5000);
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

String getContentType(String filenome) { // convert the file extension to the MIME type
  if (filenome.endsWith(".htm")) return "text/html";
  else if (filenome.endsWith(".css")) return "text/css";
  else if (filenome.endsWith(".js")) return "application/javascript";
  else if (filenome.endsWith(".ico")) return "image/x-icon";
  else if (filenome.endsWith(".json")) return "application/json";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  if (path.endsWith("/"))
  { 
    path += "index.htm";         // If a folder is requested, send the index file
    page = file1;
  }else if(path.equals("/confi.htm"))
  {
    page = file3;
  }else if(path.equals("/led.htm"))
  {
    page = file2;
  }else if(path.equals("/set.htm"))
  {
    page = file4;
  }else if(path.equals("/index.htm"))
  {
    page = file1;
  }

  String contentType = getContentType(path);            // Get the MIME type
  if(SPIFFS.begin()){
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    delay(500);
    size_t sent = serverA.streamFile(file, contentType); // And send it to the client
    file.close(); 
    SPIFFS.end();                                      // Then close the file again
    return true;
  }else{
    path += ".gz";
    File file   = SPIFFS.open(path, "r");                 // Open it
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
void loadConfigurator(const char* filenome, Config &conf){
  DynamicJsonDocument doc(2048);
  Config dados = conf;
  
  if(SPIFFS.begin()){ ///inicializa sistema de arquivos
    File file;
    if(SPIFFS.exists(filenome)){ // Verifica existencia de arquivo
      file = SPIFFS.open(filenome, "r");  //Existindo abre em modo de leitura
    }else{
      file = SPIFFS.open(filenome,"w"); /// caso contrário cria arquivo para escrita de valores.
    }
    DeserializationError error = deserializeJson(doc,file); // formata em modelo Json
    if(error) Serial.println("Failed to read file, using default config");
    
    file.close();
    SPIFFS.end();

    delay(500);
    if(strcmp(filenome,file1) == 0)
    {
    strlcpy(dados.host,doc["hostname"]|"aqua",sizeof(dados.host)); //Grava dados de hostname,
    dados.pump     = doc["Pump"];      // estado da bomba (on ou Off),
    strlcpy(dados.led, doc["Led"]|"manual", sizeof(dados.led));   // estado do Led,
    dados.corLed.r = doc["cor_r"];  ///intenciadade da cor vermelha no led,
    dados.corLed.g = doc["cor_g"]; //intencidade da cor verde no led,
    dados.corLed.b = doc["cor_b"]; //intencidade da cor azul no led,

    }else if(strcmp(filenome, file2) == 0)
    {
    strlcpy(dados.led, doc["Led"]|"manual", sizeof(dados.led));   // estado do Led,
    dados.sun      = doc["sun"];
    dados.speed    = doc["delay"]|500; //atualiza velocidade de transição na cor dos leds
    dados.corLed.r = doc["cor_r"];  ///intenciadade da cor vermelha no led,
    dados.corLed.g = doc["cor_g"]; //intencidade da cor verde no led,
    dados.corLed.b = doc["cor_b"]; //intencidade da cor azul no led,

    }else if(strcmp(filenome,file3) == 0)
    {
      copyArray(doc["pgr"],dados.programas); // Programações da bomba e 
      copyArray(doc["hr"], dados.horarios);   // horarios de ativação da bomba.
      dados.count    = doc["count"];  ///Quantidades de programas,


    }else if(strcmp(filenome,file4) == 0)
    {
      dados.ntpServ  = doc["ntpServer"]|"pool.ntp.org";
      dados.port     = doc["ntpPort"]|123;
      dados.zone     = doc["ntpZone"]|-3;

    }
   
  }

}


//Salva dados de configuração do módulo
void saveConfig(const char* filenome, Config &conf){
  Config dados = conf;
  if(SPIFFS.begin()){
    if(SPIFFS.exists(filenome)){ // Verifica se arquivo existe
      SPIFFS.remove(filenome);   // Exclui arquivo para reescrever
    }
    File file = SPIFFS.open(filenome,"w"); // Abre arquivo em modo de escrita
    if(!file){
      SPIFFS.end();
      return;
    }
    
    DynamicJsonDocument doc(2048); //cria JsonDocument para serializar dados

    if(strcmp(filenome,file1) == 0)
    {
      doc["hostname"]   = dados.host;
      doc["Pump"]       = dados.pump;
      doc["Led"]        = dados.led;
      doc["cor_r"]      = dados.corLed.r;
      doc["cor_g"]      = dados.corLed.g;
      doc["cor_b"]      = dados.corLed.b;

    }else if(strcmp(filenome,file2) == 0)
    {
      doc["delay"]      = dados.speed;
      doc["cor_r"]      = dados.corLed.r;
      doc["cor_g"]      = dados.corLed.g;
      doc["cor_b"]      = dados.corLed.b;
      doc["sun"]        = dados.sun;
      doc["Led"]        = dados.led;

    }else if(strcmp(filenome,file4) == 0)
    {
      doc["ntpServer"]  = dados.ntpServ;
      doc["ntpPort"]    = dados.port;
      doc["ntpZone"]    = dados.zone;

    }else if(strcmp(filenome,file3) == 0)
    {
      copyArray(dados.programas, doc["pgr"]);
      copyArray(dados.horarios, doc["hr"]);
      doc["count"]      = dados.count;
    
    }

    delay(500);
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