
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

function pumpSet(param1, param2){
    var xh = new XMLHttpRequest();
    xh.open("POST","/update", true);
    xh.onreadystatechange = function(){ // chama a função quando o estado mudar.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            //Requisição finalizada. Faca o processamento aqui.
        }
    } 
    var params = new FormData();
    params.append(param1, param2);
    xh.send(params);
    console.log(param1,": ", param2);
}

function init(){
document.getElementById("selector").addEventListener("input",sendCor);
document.querySelector("#pump").addEventListener("change",function(envent){
    var checkePump = document.querySelector("#pump");
    if (checkePump.checked) {
        pumpSet("pump","on");
    }else{
        pumpSet("pump","off");
    }
});
document.querySelector("#auto").addEventListener("change",function(envent){
    var auto = document.querySelector("#auto");
    var selector = document.querySelector("#selector");
    if (auto.checked) {
        selector.disabled = true;
        pumpSet("color","auto");
    }else{
        selector.disabled = false;
        pumpSet("color","handle");
    }
});
}

document.addEventListener("DOMContentLoaded",init,false);