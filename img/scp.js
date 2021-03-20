/*
    Script utilizado nas páginas index e led
    Autor: Cristiano da Rocha Silva
    Data: 12/01/2021
    Projeto: Aqua Control AP300
*/


var stsPump;
var stsLed;
//Função de Inicialização
function init(){
    var swPump = document.querySelector("#pump"); //Busca objeto de tela(Chave de acionamento da bomba)
    var ledPanel = document.querySelector("#ledColor"); // Busca objeto de tela(Painel de Led)
    var dadosJson;
    var xh = new XMLHttpRequest();
    xh.open("GET", "update", true); //Solicita abertura do arquivo de configuração
    xh.send();
    xh.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
            // atualiza pagina aqui
            console.log(this.responseText);
            dadosJson = JSON.parse(this.responseText); // Recebe dados de atualização da página
            stsPump = dadosJson.Pump; // Verifica estado da Bomba
            stsLed = dadosJson.Led;

            if(ledPanel != null){ // Caso painel de Led exista
              var colorLed = document.querySelector("#ledColor");
              var cor = [];
              cor[0] = dadosJson.cor_r;
              cor[1] = dadosJson.cor_g;
              cor[2] = dadosJson.cor_b;
              var c = "#";
              if(cor[0] <= 15){
                c += "0"+cor[0].toString(16);
              }else{
                c += cor[0].toString(16);
              }
              if(cor[1] <= 15){
                c += "0"+cor[1].toString(16);
              }else{
                c += cor[1].toString(16);
              }
              if(cor[2] <= 15){
                c += "0"+cor[2].toString(16);
              }else{
                c += cor[2].toString(16);
              }
              colorLed.value = c;

              if(stsLed.includes("manual")){ // e o estado do led seja manual
                ledPanel.disabled = false; // Habilita painel de Led para interação
              }else{
                ledPanel.disabled = true; // senão desabilita painel de Led
              }
            }

            if(swPump != null){
              if(stsPump){
                swPump.checked = true;
              }else{
                swPump.checked = false;
              }
            }

        }
    }

    ledPanel.addEventListener("input", function(){handlerLed()},false); //Monitora evento de mudança na entrada de cor
    swPump.addEventListener("change",function(){handlerPump(swPump)},false ); //Monitora envento de click no botão

};

//// Função que habilita menu superior
function myFunction() {
    var x = document.getElementById("meuMenu");
    if (x.style.display === "block") {
      x.style.display = "none";
    } else {
      x.style.display = "block";
    }
  };

//// Aciona cor do Led de forma manual
function handlerLed(){
  var colorSelected = document.querySelector("#ledColor").value;
  var xh = new XMLHttpRequest();
  xh.open("POST", "led", true);
  formdata = new FormData();
  formdata.append("cor", colorSelected);
  xh.send(formdata);
  ///console.log(colorSelected);
};

//// Acionamento Manual da Bomba
  function handlerPump(sw){
    var xh = new XMLHttpRequest();

    xh.open("POST","pump", true);
    formdata = new FormData();
    if(sw.checked){
        //console.log("Ligando Bomba");
        //Enviar Comando aqui...
        formdata.append("setPump", "on");
      
    }else{
        //console.log("Desligando Bomba");
        //Enviar Comando aqui...
        formdata.append("setPump", "off");
    }

    xh.send(formdata);
  };

window.onload = function(){
    document.querySelector("#loader").style.display = "none";
  }
  document.addEventListener("DOMContentLoaded", init, false);