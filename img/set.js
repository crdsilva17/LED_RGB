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
            dadosJson = JSON.parse(this.responseText); // Recebe dados de atualização da página

            ntpServer.value = dadosJson.ntpServer;
            port.value = dadosJson.ntpPort;
            zone.value = dadosJson.ntpZone;
        
            for(var i=0; i<4; i++){
            _ip[i] = dadosJson.ip[i];
            _gw[i] = dadosJson.gw[i];
            _sn[i] = dadosJson.sn[i];
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
    }

    if(dhc != null){
      for(var i = 0; i< dhc.length; i++){
        dhc[i].addEventListener("change",function(){meuip()}, false);
      }
    }

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