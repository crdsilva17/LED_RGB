/*
    Script utilizado nas páginas index e led
    Autor: Cristiano da Rocha Silva
    Data: 12/01/2021
    Projeto: Aqua Control AP300
*/

var stsLed;
var stsPump;
var _ip=[4];
var _gw=[4];
var _sn=[4];
//Função de Inicialização
function init(){
    var swPump = document.querySelector("#pump"); //Busca objeto de tela(Chave de acionamento da bomba)
    var ledPanel = document.querySelector("#ledColor"); // Busca objeto de tela(Painel de Led)
    var slider = document.querySelector("#delay");
    var dhc = document.querySelectorAll('input[name="dhcp"]');
    var suncheck = document.querySelector("#sun");
    var dadosJson;
    var xh = new XMLHttpRequest();

    xh.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
            // atualiza pagina aqui
            //console.log(this.responseText);
            dadosJson = JSON.parse(this.responseText); // Recebe dados de atualização da página
            stsLed = dadosJson.Led; // Verifica estado do Painel de Led
            stsPump = dadosJson.Pump; // Verifica estado da Bomba
            
            for(var i=0; i<4; i++){
            _ip[i] = dadosJson.ip[i];
            _gw[i] = dadosJson.gw[i];
            _sn[i] = dadosJson.sn[i];
            }
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
            }else{
              if(slider != null)
              slider.value = dadosJson.delay;
            }
            var div = document.querySelector("#interna");
            if(div != null){
              var radio = div.querySelectorAll('input[name="all"]');
              if(stsLed.includes("sequencia")){
                radio[0].checked = true;
              }else if(stsLed.includes("aleatorio")){
                radio[1].checked = true;
              }else if(stsLed.includes("dimmer")){
                radio[2].checked = true;
              }else{
                radio[3].checked = true;
              }
            }

            if(swPump != null){
              if(stsPump){
                swPump.checked = true;
              }else{
                swPump.checked = false;
              }
            }

            if(suncheck != null){
              suncheck.checked = dadosJson.sun;
              suncheck.addEventListener("change", function(){sun()}, false);
            }

            if(dhc[0] != null){
              dhc[1].checked = dadosJson.dhcp;
              if(dhc[0].checked == true){
                document.querySelector("#manualIP").style.display = "none";
              }else{
                document.querySelector("#manualIP").style.display = "block";
                let str1 = _ip[0];
                let str2 = _gw[0];
                let str3 = _sn[0];
                str1 += '.' + _ip[1];
                str2 += '.' + _gw[1];
                str3 += '.' + _sn[1];
                str1 += '.' + _ip[2];
                str2 += '.' + _gw[2];
                str3 += '.' + _sn[2];
                str1 += '.' + _ip[3];
                str2 += '.' + _gw[3];
                str3 += '.' + _sn[3];
                
                document.querySelector("#eIP").value = str1;
                document.querySelector("#SR").value =  str3;
                document.querySelector("#GT").value =  str2;              
              }
            }

        }
    };

    xh.open("GET", "update", true); //Solicita abertura do arquivo de configuração
    xh.send();
    if(ledPanel != null){   //Caso painel de led exista
      ledPanel.addEventListener("input", function(){handlerLed()},false); //Monitora evento de mudança na entrada de cor
    }else{
      let radioLed = document.querySelectorAll('input[name="all"]'); // Cria Array com radioButtons
      if(slider != null)
      slider.addEventListener("change", function(){altLed()}, false);
      
      for(var i = 0; i < radioLed.length; i++){
        radioLed[i].addEventListener("change", function(){altLed()}, false); // Monitora evento para cada RadioButton
      }
    }

    if(dhc != null){
      for(var i = 0; i< dhc.length; i++){
        dhc[i].addEventListener("change",function(){meuip()}, false);
      }
    }

    if(swPump != null){ // Caso botão de acionamento da bomba exista 
        swPump.addEventListener("change",function(){handlerPump(swPump)},false ); //Monitora envento de click no botão
    }

};

function sun(){
  var suncheck = document.querySelector("#sun");
  var xh = new XMLHttpRequest();
  var formdata = new FormData();
  formdata.append("sun", suncheck.checked);
  xh.open("POST", "led",true);
  xh.send(formdata);
};

function saveDhcp(){
  var xh = new XMLHttpRequest();
  var json;
  var dhc = document.querySelectorAll('input[name="dhcp"');
  xh.onreadystatechange = function(){
    if (this.readyState == 4 && this.status == 200){
      window.alert("CONFIGURAÇÃO SALVA!");
    }
  };
  xh.open("POST", "meuIp", true);
  formdata = new FormData();
  if(dhc[1].checked == 1){
    _ip = document.querySelector("#eIP").value.split(".");
    _sn = document.querySelector("#SR").value.split(".");
    _gw = document.querySelector("#GT").value.split(".");
  }else{
    _ip = [0,0,0,0];
    _gw = [0,0,0,0];
    _sn = [0,0,0,0];
  }
  json = '{"ip":['+_ip[0]+','+_ip[1]+','+_ip[2]+','+_ip[3]+'],"gw":[';
  json += _gw[0]+','+_gw[1]+','+_gw[2]+','+_gw[3]+'],"sn":[';
  json += _sn[0]+','+_sn[1]+','+_sn[2]+','+_sn[3]+'],"dhcp":'+dhc[1].checked+'}';
  formdata.append("meuip",json);
  xh.send(formdata);
};

function cancelDhcp(){
  window.location.replace("/set.htm");
};


function meuip(){
  var dhc = document.querySelectorAll('input[name="dhcp"]');
  if(dhc[1].checked == 1){
    document.querySelector("#manualIP").style.display = "block";
    let str1 = _ip[0];
    let str2 = _gw[0];
    let str3 = _sn[0];
    str1 += '.' + _ip[1];
    str2 += '.' + _gw[1];
    str3 += '.' + _sn[1];
    str1 += '.' + _ip[2];
    str2 += '.' + _gw[2];
    str3 += '.' + _sn[2];
    str1 += '.' + _ip[3];
    str2 += '.' + _gw[3];
    str3 += '.' + _sn[3];
    
    document.querySelector("#eIP").value = str1;
    document.querySelector("#SR").value =  str3;
    document.querySelector("#GT").value =  str2; 
  }else{
    document.querySelector("#manualIP").style.display = "none";
  }
};

// Função que altera estado do led
function altLed(){
  var button = document.querySelector('input[name="all"]:checked').getAttribute("ID");
  var slider = document.querySelector("#delay");
  var xh = new XMLHttpRequest();
  xh.open("Post", "led", true);
  formdata = new FormData();
  formdata.append("estado", button);
  formdata.append("delay",slider.value);
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

  ///Função que reinicia o dispositivo para atualizar dados salvos recente.
  function reset(){
    var xh = new XMLHttpRequest();
    var formdata = new FormData();
    xh.open("POST", "reset", true );
    formdata.append("reset", "again");
    xh.send(formdata);    
  };

  ///Função que reinicia dispositivo para padrão de fábrica.
  function resetDef(){
    var xh = new XMLHttpRequest();
    var formdata = new FormData();
    xh.open("POST","reset", true);
    formdata.append("reset", "default");
    xh.send(formdata); 
  };


  document.addEventListener("DOMContentLoaded", init, false);