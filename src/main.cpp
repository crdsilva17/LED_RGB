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
const char* SSID = "YOURSSID"; ///< Ssid Network
const char* PASSWD =  "YOURPASSWORD"; ///< Password for Network
bool hand = true; /*!< handle manual or automatic action*/
unsigned long setTime = 1000; /*!< set time delay */
unsigned long previusTime = 0; /*!< receive previus time for trigger delay */
File uploadFile; /*!< Store data for SPIFFS readed*/
int pinR = 12; /*!< Set pin red as number 12 */
int pinG = 13; /*!< Set pin green as number 13 */
int pinB = 14; /*!< Set pin blue as number 14 */
int pump = 5; ///< define pin of pump
int sunLight = 4; /// define ldr sensor
String page = ""; /*!< Store page HTML */


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
 * @fn handleRoot()
 * @brief manipulates index.htm file
 * 
 *        send response a first requisition
 * 
 * @param  - void parameters
 * @return void
 * */
void handleRoot();
/**
 * @fn handleCss()
 * @brief set style page
 * 
 *        set style on page with file.css
 * 
 * @param - void parameters
 * @return void
 * */
void handleCss();
/**
 * @fn handleJs()
 * @brief Javascript source
 * 
 *        set script on the page whit file.js
 * 
 * @param - void parameters
 * @return void
 * **/
void handleJs();
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
 * @fn readFile(String path)
 * @brief Read file SPIFFS
 * 
 *        Open and read all files in the flash system
 * 
 * ###Variables
 * 
 * **String** *content* ---- //return variable\n
 * **File** *file*   ----     //add file data
 * 
 * @param path - File name
 * @return String read from file
 * @var File file
 * */
String readFile(String path);


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
  Serial.print("Connect ");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  serverA.on("/", [](){
    if(!serverA.authenticate(user,pass)){ 
      return serverA.requestAuthentication(); 
    }
    handleRoot(); 
  });

  serverA.on("/st.css",handleCss); 
  serverA.on("/scp.js",handleJs); 
  serverA.on("/update",handleAction); 
  serverA.on("/favicon.ico",HTTP_GET,[](){
  page = readFile("/favicon.ico");
  serverA.send(200,"image/x-icon",page);
    
    });
  serverA.onNotFound(handleNotFound);

  
  MDNS.begin(host);
  httpUpdater.setup(&serverA, update_path, user, pass);
  serverA.begin();

  MDNS.addService("http", "tcp", 80);

}



void loop() {
  
  if(hand != true){
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
  
  serverA.handleClient();
  MDNS.update();

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
 * Set color led
 * */
void led(RGB colorname){
  analogWrite(pinR,colorname.r);
  analogWrite(pinG,colorname.g);
  analogWrite(pinB,colorname.b);
}

/**
 * Read File
 * */
String readFile(String path){ // Function to read all files of system memory
// creates return variable
    String content = ""; 
   if(SPIFFS.begin()){ //begin files class
    if(SPIFFS.exists(path)){ //verify if file exists
    File file = SPIFFS.open(path,"r"); //open file in read mode 
    content = file.readString(); //read open file
    file.close(); // close file
    }
    SPIFFS.end(); //finish files class
  }
  return content; // return file data
}

void handleRoot(){ // call function to principal page
  page = readFile("/index.htm");
  serverA.send(200,"text/html",page);
}
/**
 * Set stylesheet of page
 * */
void handleCss(){ // function to creates style of webpage
  page = readFile("/st.css");
  serverA.send(200,"text/css",page);
}
/**
 * set script of page
 * */
void handleJs(){ // function to call javascript file
  page = readFile("/scp.js");
  serverA.send(200,"text/javascript",page);
}
/**
 * handle error page
 * */
void handleNotFound(){ // function to handle webpage error
  serverA.send(404,"text/plain","ERROR! Page Not Found!!!");
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
  }

  /// On Off Pump
  if(serverA.argName(0).equals("pump")){
    if(serverA.arg(0).equals("on")){
      digitalWrite(pump,HIGH);
    }else if(serverA.arg(0).equals("off")){
      digitalWrite(pump,LOW);
    }
  }

  if(serverA.argName(0).equals("color")){
    if(serverA.arg(0).equals("auto")){
      hand = false;
      color = {15,0,0};
    }else if(serverA.arg(0).equals("handle")){
      hand = true;
    }
  }
  serverA.send(200);
}

