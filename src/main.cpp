#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#define SSID  "Virus" // Ssid Network
#define PASSWD  "aoc18090" // Password for Network
#define pump   D8 // define pin of pump
#define sunLight D4 // define ldr sensor

const char* user = "admin"; //username
const char* pass = "senha123"; //password
const char* host = "aqua"; //hostname - aqua.local
const char* update_path = "/firmware";

bool hand = true;
long t = 0L;

File uploadFile;



int pinR = 12; //pin red color
int pinG = 13; //pin green color
int pinB = 14; //pin blue color


String page = ""; // variable that we can set the html page

ESP8266WebServer serverA(80); //Instance Server
ESP8266HTTPUpdateServer httpUpdater;



void handleRoot();
void handleCss();
void handleJs();
void handleAction();
void handleNotFound();
String readFile(String path);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinR, OUTPUT); // Set pin as Output
  pinMode(pinG, OUTPUT); // Set pin as Output
  pinMode(pinB, OUTPUT); // Set pin as Output
  pinMode(pump, OUTPUT);// Set  pin pump as Output
  pinMode(sunLight, INPUT); // Set pin sensor sun as Input

  digitalWrite(pump, LOW); // Set pump off


  analogWriteRange(10); // Range 0 at 1023
  analogWriteFreq(60); // Set Frequency


  analogWrite(pinR,0); // pin off
  analogWrite(pinG,0); // pin off
  analogWrite(pinB,0); //pin off

  WiFi.mode(WIFI_STA); // Station mode
  WiFi.begin(SSID,PASSWD); //Connect network
  Serial.print("Connect ");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  serverA.on("/", [](){
    if(!serverA.authenticate(user,pass)){ // verifica login e senha 
      return serverA.requestAuthentication(); // retorna se não estiver correto
    }
    handleRoot(); //trata chamadas para arquivo principal
  });

  serverA.on("/st.css",handleCss); // trata chamadas para folha de estilo
  serverA.on("/scp.js",handleJs); // trata chamadas para script
  serverA.on("/update",handleAction); // trata chamadas para ação de acionamento do led
  serverA.on("/favicon.ico",HTTP_GET,[](){
  page = readFile("/favicon.ico");
  serverA.send(200,"image/x-icon",page);
    
    });
  serverA.onNotFound(handleNotFound);// trata erro de paginas

  
  MDNS.begin(host);
  httpUpdater.setup(&serverA, update_path, user, pass);
  serverA.begin();

  MDNS.addService("http", "tcp", 80);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  serverA.handleClient();
  MDNS.update();
  if(!hand){
    if(t > 1000L){t = 0L;}
    analogWrite(pinR, 255);
    analogWrite(pinG, 0);
    analogWrite(pinB, 0);
    delay(t);
    analogWrite(pinR, 0);
    analogWrite(pinG, 255);
    analogWrite(pinB, 0);
    delay(t);
    analogWrite(pinR, 0);
    analogWrite(pinG, 0);
    analogWrite(pinB, 255);
    delay(t);
    analogWrite(pinR, 255);
    analogWrite(pinG, 255);
    analogWrite(pinB, 0);
    delay(t);
    analogWrite(pinR, 255);
    analogWrite(pinG, 0);
    analogWrite(pinB, 255);
    delay(t);
    analogWrite(pinR, 0);
    analogWrite(pinG, 255);
    analogWrite(pinB, 0);
    delay(t);
    analogWrite(pinR, 0);
    analogWrite(pinG, 255);
    analogWrite(pinB, 255);
    delay(t);
    analogWrite(pinR, 255);
    analogWrite(pinG, 10);
    analogWrite(pinB, 0);
    delay(t);
    analogWrite(pinR, 255);
    analogWrite(pinG, 0);
    analogWrite(pinB, 10);
    delay(t);
    analogWrite(pinR, 255);
    analogWrite(pinG, 10);
    analogWrite(pinB, 10);
    delay(t);
    analogWrite(pinR, 10);
    analogWrite(pinG, 255);
    analogWrite(pinB, 0);
    delay(t);
    analogWrite(pinR, 0);
    analogWrite(pinG, 255);
    analogWrite(pinB, 10);
    delay(t);
    analogWrite(pinR, 10);
    analogWrite(pinG, 255);
    analogWrite(pinB, 10);
    delay(t);
    analogWrite(pinR, 10);
    analogWrite(pinG, 0);
    analogWrite(pinB, 255);
    delay(t);
    analogWrite(pinR, 0);
    analogWrite(pinG, 10);
    analogWrite(pinB, 255);
    delay(t);
    analogWrite(pinR, 10);
    analogWrite(pinG, 10);
    analogWrite(pinB, 255);
    delay(t);
    analogWrite(pinR, 255);
    analogWrite(pinG, 255);
    analogWrite(pinB, 255);
    delay(t);
    t++;
    }

}

String readFile(String path){ // Função de leitura dos arquivos na memória flash
   String content = ""; // cria variavel de retorno
   LittleFSConfig cfg;
   cfg.setAutoFormat(false);
   LittleFS.setConfig(cfg);
   
   if(LittleFS.begin()){ // inicia classe de arquivo
    if(LittleFS.exists(path)){ // verifica se o arquivo solicitado existe
    File file = LittleFS.open(path,"r"); // abre o arquivo em modo de leitura
    content = file.readString(); // realiza a leitura do arquivo aberto
    file.close(); // fecha arquivo
    }
    LittleFS.end(); // finaliza classe de arquivo
  }
  return content; // retorna dados do arquivo
}

void handleRoot(){ // Função que chama a pagina principal
  page = readFile("/index.htm");
  serverA.send(200,"text/html",page);
}

void handleCss(){ // Função que cria o estilo da pagina web
  page = readFile("/st.css");
  serverA.send(200,"text/css",page);
}

void handleJs(){ // Função que realiza a chamada do arquivo javascript
  page = readFile("/scp.js");
  serverA.send(200,"text/javascript",page);
}

void handleNotFound(){ // Função que indica que a pagina web não foi encontrada
  serverA.send(404,"text/plain","ERROR! Page Not Found!!!");
}

/*
 Função que trata as ações enviadas pelo cliente
*/
void handleAction(){

  if(hand){
  if(serverA.argName(0).equals("R")){
    analogWrite(pinR,map(serverA.arg(0).toInt(),0,255,0,1023)); //Aciona o PWM para a saída Vermelha
  }
  if(serverA.argName(1).equals("G")){
    analogWrite(pinG,map(serverA.arg(1).toInt(),0,255,0,1023)); // Aciona o PWM para a saída Verde
  }
  if(serverA.argName(2).equals("B")){
    analogWrite(pinB,map(serverA.arg(2).toInt(),0,255,0,1023)); // Aciona o PWM para a saída Azul
  }
  }

  // On Off Pump
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
    }else if(serverA.arg(0).equals("handle")){
      hand = true;
    }
  }
  serverA.send(200);
}

