/*
    Script utilizado nas páginas index e led
    Autor: Cristiano da Rocha Silva
    Data: 12/01/2021
    Projeto: Aqua Control AP300
*/

var _ip=[4];
var _gw=[4];
var _sn=[4];
//Função de Inicialização
function init(){
    var dhc = document.querySelectorAll('input[name="dhcp"]');
    var port = document.querySelector('input[name="ntpPort"]');
    var zone = document.querySelector('input[name="ntpTimezone"]');
    var ntpServer = document.querySelector('input[name="ntpServer"]');
    var dadosJson;
    var xh = new XMLHttpRequest();
    xh.open("GET", "update", true); //Solicita abertura do arquivo de configuração
    xh.send();
    xh.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
            // atualiza pagina aqui
            console.log(this.responseText);

            try {
              dadosJson = JSON.parse(this.responseText); // Recebe dados de atualização da página
              ntpServer.value = dadosJson.ntpServer;
              port.value = dadosJson.ntpPort;
              zone.value = dadosJson.ntpZone;
            } catch (error) {
              console.log(error);
            }
           
            update();

        }
    }
};

function cancelDhcp(){
  window.location.replace("/set.htm");
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

  ///Função que reinicia o dispositivo para atualizar dados salvos recente.
  function reset(){
    var xh = new XMLHttpRequest();
    var formdata = new FormData();
    xh.onreadystatechange = function(){
      if(this.readyState==4 && this.status==200){
        var body = document.body;
        var h1 = document.createElement("h1");
        var texto = document.createTextNode("Reiniciando o dispositivo....");
        h1.appendChild(texto);
        body.appendChild(h1);
      }
    }
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

  function update(){
    var timer = document.querySelector("#rAtual");
    var xh = new XMLHttpRequest();
    xh.open("GET", "updateClock", true);
    xh.send();

    xh.onreadystatechange = function(){
      if (this.readyState == 4 && this.status == 200){
        timer.value = this.responseText;
        console.log(this.responseText);
      }
    }
  };

  function saveH(){
    var timer = document.querySelector("#rAtual");
    var port = document.querySelector('input[name="ntpPort"]');
    var zone = document.querySelector('input[name="ntpTimezone"]');
    var ntpServer = document.querySelector('input[name="ntpServer"]');
    var xh = new XMLHttpRequest();   
    var formdata = new FormData();
    formdata.append("ntpServer", ntpServer.value);
    formdata.append("port", port.value);
    formdata.append("ntpZone", zone.value);
    formdata.append("hora", timer.value);
    xh.open("POST", "clock", true);
    xh.send(formdata);
  };


window.onload = function(){
    document.querySelector("#loader").style.display = "none";
  }
  document.addEventListener("DOMContentLoaded", init, false);