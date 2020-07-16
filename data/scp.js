/**
 * @fn senCor()
 * @brief Envia cor padrão RGB
 * 
 *          coleta cor selecionada pelo usuário e
 *          envia para o servidor.
 * 
 * */
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

/**
 * @fn pumpSet(param1, param2, param3)
 * @brief envia status da bomba
 * 
 *          Responsável por ligar e desligar
 *          bomba da piscina.    
 *  
 * @param {*} param1 
 * @param {*} param2 
 * @param {*} param3 
 * */
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
/**
 * @fn pumpSet1(param1, param2, param3, param4)
 * @brief Altera valores auto/manual 
 * 
 *          configura tempos para ligar e
 *          desligar bomba automaticamente
 * 
 * 
 * @param {*} param1 
 * @param {*} param2 
 * @param {*} param3 
 * @param {*} param4 
 * */
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

/**
 * @fn init()
 * @brief Função inicial
 * 
 *          roda após página carregar
 * 
 * */
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
updateValue();

document.querySelector(".hour").addEventListener("change",set_hour);
document.querySelector(".min").addEventListener("change",set_min);
document.querySelector(".sec").addEventListener("change",set_sec);
document.querySelector(".hour1").addEventListener("change",set_hour);
document.querySelector(".min1").addEventListener("change",set_min);
document.querySelector(".sec1").addEventListener("change",set_sec);

}

/**
 * @fn updateValue()
 * @brief Atualizar dados
 * 
 *          mantêm dados atualizados para os 
 *          clientes conectados.
 *          Atualização roda a cada 1 segundo.
 * */
function updateValue(){
    setTimeout(updateValue,1000);
    var xhjson = new XMLHttpRequest();
    xhjson.open("POST","/update",true);
    xhjson.onreadystatechange = function (){
    if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            // Requisição finalizada. Faça o processamento aqui.
            var response = JSON.parse(xhjson.responseText);
            var ledAuto = response.led;
            var pumpAuto = response.pump;
            var red = parseInt(response.red);
            var green = parseInt(response.green);
            var blue = parseInt(response.blue);
            var pro1 = response.prog1;
            var pro2 = response.prog2;
            var range = response.range;

            var auto = document.querySelector("#auto");
            var selector = document.querySelector("#selector");  
            var speed = document.querySelector("#myRange");
            var pump = document.querySelector("#pump");
            console.log(pro1);
            console.log(pro2);

            var time1 = pro1.split(":");
            var time2 = pro2.split(":");

        var cor = "#";
        if(red < 10){
            cor += "0" + red;
        }else{
            cor += red.toString(16);
        }
        if(green < 10){
            cor += "0" + green;
        }else{
            cor +=  green.toString(16);
        }
        if(blue < 10){
            cor += "0" + blue;
        }else{
            cor += blue.toString(16);
        }
        if(selector.value != cor){
        selector.value = cor;
        }
        if(auto.checked == true){
        speed.value = range;
        }
        
        if(pumpAuto != pump.checked){
        if(pumpAuto == true){
        pump.checked = true;
        }else{
            pump.checked = false;
        }
        }
if(pump.checked == true){
        if(document.querySelector(".hour").value != time1[0]){
        document.querySelector(".hour").value = time1[0];
        }
        if(document.querySelector(".min").value != time1[1]){
        document.querySelector(".min").value = time1[1];
        }
        if(document.querySelector(".sec").value != time1[2]){
        document.querySelector(".sec").value = time1[2];
        }

        if( document.querySelector(".hour1").value != time2[0]){
        document.querySelector(".hour1").value = time2[0];
        }
        if(document.querySelector(".min1").value != time2[1]){
        document.querySelector(".min1").value = time2[1];
        }
        if(document.querySelector(".sec1").value != time2[2]){
        document.querySelector(".sec1").value = time2[2];
        }
    }
        if(pump.checked){
            document.querySelector("#setPump").disabled = true;
        }else{
            document.querySelector("#setPump").disabled = false;
        }

        if(ledAuto == auto.checked){
        if(ledAuto == true){
        auto.checked = false;
         }else{
        auto.checked = true;
         }
        }
          if (auto.checked) {
        selector.disabled = true;
          }else{
        selector.disabled = false;
          }
    }       

    }
    var form = new FormData();
    form.append("update","1");
    xhjson.send(form);
}

/**
 * @fn moveClock()
 * @brief Relógio
 * 
 *          Mantêm relógio atualizado
 *         Roda a cada 1 segundo.
 * 
 * 
 * */
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