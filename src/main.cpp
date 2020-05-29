#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#define SSID  "Virus" // Ssid Network
#define PASSWD  "aoc18090" // Password for Network

int pinR = 12; //pin red color
int pinG = 13; //pin green color
int pinB = 14; //pin blue color


String page = ""; // variable that we can set the html page

ESP8266WebServer serverA(80); //Instance Server

const char* user = "admin"; //username
const char* pass = "senha123"; //password

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

  analogWriteRange(8); // Range 0 at 255
  analogWriteFreq(5000); // Set Frequency

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
  serverA.on("/update",HTTP_GET,handleAction); // trata chamadas para ação de acionamento do led
  serverA.onNotFound(handleNotFound); // trata erro de paginas
  

  serverA.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  serverA.handleClient();
}

String readFile(String path){ // Função de leitura dos arquivos na memória flash
   String content = ""; // cria variavel de retorno
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
  if(serverA.argName(0).equals("R")){
    analogWrite(pinR,serverA.arg(0).toInt()); //Aciona o PWM para a saída Vermelha
  }
  if(serverA.argName(1).equals("G")){
    analogWrite(pinG,serverA.arg(1).toInt()); // Aciona o PWM para a saída Verde
  }
  if(serverA.argName(2).equals("B")){
    analogWrite(pinB,serverA.arg(2).toInt()); // Aciona o PWM para a saída Azul
  }
}