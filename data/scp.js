function sendCor(){

    var xh = new XMLHttpRequest();
    xh.open("POST","/update", true);
    xh.onreadystatechange = function() { // Chama a função quando o estado mudar.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            // Requisição finalizada. Faça o processamento aqui.
        }
    }

var cor = document.getElementById("selector").value;
var R = parseInt(cor.substring(1,3),16);
var G = parseInt(cor.substring(3,5),16);
var B = parseInt(cor.substring(5,8),16);
var params = new FormData();
params.append("R",R);
params.append("G",G);
params.append("B",B);
console.log("RGB(",R,",",G,",",B,")");
xh.send(params);
}

function pumpSet(param1, param2, param3){
    var xh = new XMLHttpRequest();
    xh.open("POST","/update", true);
    xh.onreadystatechange = function(){ // chama a função quando o estado mudar.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            //Requisição finalizada. Faca o processamento aqui.
        }
    } 
    var params = new FormData();
    params.append(param1, param2);
    params.append("speed",param3);
    xh.send(params);
    console.log(param1,": ", param2);
    console.log("speed: ", param3);
}

function pumpSet1(param1, param2, param3, param4){
    var xh = new XMLHttpRequest();
    xh.open("POST","/update", true);
    xh.onreadystatechange = function(){ // chama a função quando o estado mudar.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            //Requisição finalizada. Faca o processamento aqui.
        }
    } 
    var params = new FormData();
    params.append(param1, param2);
    params.append("prog1",param3);
    params.append("prog2",param4);
    xh.send(params);
    console.log(param1," auto: ", param2);
}

function init(){
document.getElementById("selector").addEventListener("input",sendCor);
document.querySelector("#pump").addEventListener("change",function(envent){
    var checkePump = document.querySelector("#pump");
    time_liga = document.querySelector(".hour").value + ":" + document.querySelector(".min").value + ":" + document.querySelector(".sec").value;
    time_desl = document.querySelector(".hour1").value + ":" + document.querySelector(".min1").value + ":" + document.querySelector(".sec1").value;
    if (checkePump.checked) {
        if(time_desl != time_liga){
        console.log("Prog1: ",time_liga);
        console.log("Prog2: ", time_desl);
        document.querySelector("#setPump").disabled = true;
        pumpSet1("pump","on",time_liga, time_desl);
        }else{
            checkePump.checked = false;
            alert("Tempos devem ser diferentes");
        }
    }else{
        document.querySelector("#setPump").disabled = false;
        pumpSet1("pump","off", time_liga, time_desl);
    }
});
document.querySelector("#auto").addEventListener("change",function(envent){
    var auto = document.querySelector("#auto");
    var selector = document.querySelector("#selector");  
    var speed = document.querySelector("#myRange").value;
    if (auto.checked) {
        selector.disabled = true;
        pumpSet("color","auto", speed);
    }else{
        selector.disabled = false;
        pumpSet("color","handle", speed);
    }
});
moveClock();

document.querySelector(".hour").addEventListener("change",set_hour);
document.querySelector(".min").addEventListener("change",set_min);
document.querySelector(".sec").addEventListener("change",set_sec);
document.querySelector(".hour1").addEventListener("change",set_hour);
document.querySelector(".min1").addEventListener("change",set_min);
document.querySelector(".sec1").addEventListener("change",set_sec);

}
function moveClock(){
    dateNow = new Date();
    hora = dateNow.getHours();
    minutos = dateNow.getMinutes();
    segundos = dateNow.getSeconds();

    str_hora = new String(hora);
    if(str_hora.length == 1){
        hora = "0"+ hora;
    } 

    str_minuto = new String(minutos);
    if(str_minuto.length == 1){
        minutos = "0" + minutos;
    }

    str_segundo = new String(segundos);
    if(str_segundo.length == 1){
        segundos = "0" + segundos;
    }


    sendTime = hora + ":" + minutos + ":" + segundos;
    document.getElementById("relogio").value = sendTime;
    setTimeout(moveClock,1000);
}

function set_hour(envent){
    if(parseInt(this.value,10)<10)this.value='0'+this.value;
}

function set_min(event){
    if(parseInt(this.value,10)<10)this.value='0' + this.value;
}

function set_sec(event){
    if(parseInt(this.value,10)<10)this.value='0' + this.value;
}

function setPump(){
    var xh = new XMLHttpRequest();
    xh.open("POST","/update", true);
    xh.onreadystatechange = function(){ // chama a função quando o estado mudar.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            //Requisição finalizada. Faca o processamento aqui.
        }
    }
    var form = new FormData();
    form.append("setPump","set");
    console.log(form.get("setPump"));
    xh.send(form);

}

document.addEventListener("DOMContentLoaded",init,false);