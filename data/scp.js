/*
    Script utilizado nas páginas index e led
    Autor: Cristiano da Rocha Silva
    Data: 12/01/2021
    Projeto: Aqua Control AP300
*/

var stsLed;
var stsPump;

//Função de Inicialização
function init(){
    let swPump = document.querySelector("#pump"); //Busca objeto de tela(Chave de acionamento da bomba)
    let ledPanel = document.querySelector("#ledColor"); // Busca objeto de tela(Painel de Led)
    let dadosJson;
    let xh = new XMLHttpRequest();

    xh.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
            // atualiza pagina aqui
            dadosJson = JSON.parse(this.responseText); // Recebe dados de atualização da página
            stsLed = dadosJson.Led; // Verifica estado do Painel de Led
            stsPump = dadosJson.Pump; // Verifica estado da Bomba

            if(ledPanel != null){ // Caso painel de Led exista
              if(stsLed == "manual"){ // e o estado do led seja manual
                ledPanel.disabled = false; // Habilita painel de Led para interação
              }else{
                ledPanel.disabled = true; // senão desabilita painel de Led
              }
            }

        }
    };

    xh.open("GET", "config.json", true); //Solicita abertura do arquivo de configuração
    xh.send();
    if(ledPanel != null){   //Caso painel de led exista
      ledPanel.addEventListener("input", function(){handlerLed()},false); //Monitora evento de mudança na entrada de cor
    }else{
      let radioLed = document.querySelectorAll('input[name="all"]'); // Cria Array com radioButtons
      
      for(let i = 0; i < radioLed.length; i++){
        radioLed[i].addEventListener("change", function(){altLed()}, false); // Monitora evento para cada RadioButton
      }
    }

    if(swPump != null){ // Caso botão de acionamento da bomba exista 
        swPump.addEventListener("change",function(){handlerPump(swPump)},false ); //Monitora envento de click no botão
    }
};

// Função que altera estado do led
function altLed(){
  var button = document.querySelector('input[name="all"]:checked').getAttribute("ID");
  let xh = new XMLHttpRequest();
  xh.open("Post", "led", true);
  formdata = new FormData();
  formdata.append("estado", button);
  xh.send(formdata);
 
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
  let colorSelected = document.querySelector("#ledColor").value;
  let xh = new XMLHttpRequest();
  xh.open("POST", "led", true);
  formdata = new FormData();
  formdata.append("color", colorSelected);
  xh.send(formdata);
  ///console.log(colorSelected);
};

//// Acionamento Manual da Bomba
  function handlerPump(sw){
    let xh = new XMLHttpRequest();

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


  document.addEventListener("DOMContentLoaded", init, false);