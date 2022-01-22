/*
    Script utilizado nas páginas index e led
    Autor: Cristiano da Rocha Silva
    Data: 12/01/2021
    Projeto: Aqua Control AP300
*/


//Função de Inicialização
function init(){
    var slider = document.querySelector("#delay");
    var suncheck = document.querySelector("#sun");
    var stsLed;
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
            stsLed = dadosJson.Led; // Verifica estado do Painel de Led
            } catch (error) {
              console.log(error);
            }
            
            if(stsLed.includes("sequencia")){
                radio[0].checked = true;
              }else{
                radio[1].checked = true;
              }
            
              if(slider != null){
              slider.value = dadosJson.delay;
            }
            if(suncheck != null){
                suncheck.checked = dadosJson.sun;
              }
        }
    }
        var div = document.querySelector("#interna");
        var radio = div.querySelectorAll('input[name="all"]');

    if(suncheck != null){
      suncheck.addEventListener("change", function(){altLed()}, false);
    }
     
      if(slider != null)
      slider.addEventListener("change", function(){altLed()}, false);
      
      let radioLed = document.querySelectorAll('input[name="all"]'); // Cria Array com radioButtons
      for(var i = 0; i < radioLed.length; i++){
        radioLed[i].addEventListener("change", function(){altLed()}, false); // Monitora evento para cada RadioButton
      }
};

// Função que altera estado do led
function altLed(){
  var button = document.querySelector('input[name="all"]:checked').getAttribute("ID");
  var suncheck = document.querySelector("#sun");
  var slider = document.querySelector("#delay");
  var xh = new XMLHttpRequest();
  xh.open("Post", "led", true);
  formdata = new FormData();
  formdata.append("estado", button);
  formdata.append("delay",slider.value);
  if(suncheck.checked){
  formdata.append("sun", "1");
  }else{
    formdata.append("sun", "0");
  }
  console.log("Estado: " + button);
  console.log("delay: " + slider.value);
  console.log("Sun: " + suncheck.checked);
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

window.onload = function(){
    document.querySelector("#loader").style.display = "none";
  }
  document.addEventListener("DOMContentLoaded", init, false);