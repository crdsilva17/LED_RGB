
//Variaveis Globais
/**
 * @global
 */
var btn_add = document.querySelector("#btn-add");
var local = document.querySelector("#local");
var prog = [[false,false,false,false,false,false,false],[false,false,false,false,false,false,false],[false,false,false,false,false,false,false],[false,false,false,false,false,false,false],[false,false,false,false,false,false,false]];
var hor = [5][2];
var valor = 1;
var MAX = 6;
var count = 0;


//Class que cria tabela de horários
/**
 * @class  tableCreate
 * @param {*} index 
 */
function tableCreate(index){
    this.index = index;

    /**
     * @constructor init
     */
    this.init = function () {
        var nova_tabela = document.querySelector("#template1").cloneNode(true);
        div = nova_tabela.content.querySelector("div");
        div.id = "t" + this.index;
        titulo = nova_tabela.content.querySelector("th");
        titulo.textContent = "Programação " + this.index;
        semana = nova_tabela.content.querySelector("#check");
        semana.innerHTML =
       "<td rowspan='2'><input type='checkbox' name='0' class='p1' id='d" + this.index + "'></td>\
        <td rowspan='2'><input type='checkbox' name='1' class='p1' id='s" + this.index +"'></td>\
        <td rowspan='2'><input type='checkbox' name='2'   class='p1' id='t" + this.index +"'></td>\
        <td rowspan='2'><input type='checkbox' name='3'  class='p1' id='qa" + this.index +"'></td>\
        <td rowspan='2'><input type='checkbox' name='4'  class='p1' id='qi" + this.index +"'></td>\
        <td rowspan='2'><input type='checkbox' name='5'   class='p1' id='se" + this.index +"'></td>\
        <td rowspan='2'><input type='checkbox' name='6'  class='p1' id='sa" + this.index +"'></td>";
        hliga = nova_tabela.content.querySelector("#lig");
        hdesl = nova_tabela.content.querySelector("#desl");
        hliga.id = hliga.id + this.index;
        hdesl.id = hdesl.id + this.index;
        btn_cancel = nova_tabela.content.querySelector("#del");
        btn_cancel.innerHTML = 
        "<input class='btn2' id = 'b"+this.index+"' type='button' value='Deletar' onclick='delet("+this.index+")'>";
        var novaTabela = document.importNode(nova_tabela.content, true);
        local.appendChild(novaTabela);
        
    }

};


//Função de inicialização
function init(){
  let xh = new XMLHttpRequest();

    xh.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200){
            // atualiza pagina aqui
            console.log(this.responseText);
            dadosJson = JSON.parse(this.responseText); // Recebe dados de atualização da página
            var obj = dadosJson.pgr;
            var obj2 = dadosJson.hr;
            count = dadosJson.count;

            for(var i=0; i < count; i++){
              updateTabela();
              var idLocal = document.querySelector("#local");
              var tab = idLocal.querySelector('#t'+(i+1));
              var ch = tab.querySelectorAll('input[type="checkbox"');
                for(var j=0; j<7; j++){
                  ch[j].checked = obj[i][j];
                }
              var time = tab.querySelectorAll('input[type="time"]');
              console.log(time);
              var l = 0;
                    for(var k=0; k<2; k++){
                      console.log("time["+l+"] = obj2["+i+"]["+k+"] = "+obj2[i][k]);
                      time[l].value = obj2[i][k];
                      l++;
                    }
            }
        }
      }
    xh.open("POST", "update");
    xh.send();
};

//Função que envia valores para o servidor
function save(){
  var checkId = document.querySelectorAll('input[type="checkbox"]');
  var tam = checkId.length / 7;
  let xh = new XMLHttpRequest();
  let formdata = new FormData();
  setH = document.querySelectorAll('input[type="time"]');
  var k=0;
  var json = '{"pgr":[';
  for(var i=0; i < tam; i++){
    json += "[";
    for(var j=0; j < checkId.length/tam; j++){
      prog[i][j] = checkId[k].checked;
      json += checkId[k].checked;
      if(j < 6)
      json += ",";
      k++;
    }
    json += "]";
    if(i < tam - 1)
    json += ",";
  }
  json += "]}";
  
  var k = 0;
  var json2 = '{"hr":[';
  for(var l=0; k < setH.length; l++){
    if(k > 1) json2 += ",";
    json2 += "[";
    for(var i=0; i<2; i++){
      json2 += '"';
      console.log(k);
      console.log(setH[k]);
      json2 += setH[k].value;
      k++;
      json2 += '"';
      if(i < 1) json2 += ",";
    }
    json2 += "]";
  }
  json2 +="]}";

  console.log("count: " + count);
  var json3 = '{"count":' + count+'}' ;
  formdata.append("prog", json);
  formdata.append("hora",json2);
  formdata.append("count",json3)
  for(item of formdata.entries()){
    console.log(item[0] + ":" + item[1]);
  }
  xh.open("POST", "programa", true);
  xh.send(formdata);
  
};

/**
 * @function delet
 * @param {*} t 
 */
//Deleta programas e atualiza dados no servidor
function delet(t){
    var tam = t;
    childEl = document.querySelector('#t'+t);
    parentEl = childEl.parentNode;
    parentEl.removeChild(childEl);
    for (t; t < valor-1; t++) {
        postEl = document.querySelector('#t'+(t + 1));
        postEl.id ="t" + t;
        childEl =  postEl.lastElementChild;
        childEl.querySelector("th").textContent = "Programação " + t;
        btn = document.querySelector("input[id='b"+(t+1)+"']");
        btn.setAttribute('onclick', 'delet('+t+')');
        btn.id = "b"+t;
        hliga = childEl.querySelectorAll('input[type="time"]');
        hliga[0].id = "lig" + t;
        hliga[1].id = "desl" + t;
        chEl = childEl.querySelectorAll('input[type="checkbox"');
        for(i = 0; i < chEl.length; i++){
          chEl[i].setAttribute("id",chEl[i].id.substr(0, chEl[i].id.indexOf(t+1)) + t);
        }
    }
    console.log(tam);
    switch(tam){
      case 1:
        prog.splice(0, 1);
        break;
      case 2:
        prog.splice(1, 1);
        break;
      case 3:
        prog.splice(3, 1);
        break;
      case 4:
        prog.splice(4, 1);
        break;
      case 5:
        prog.splice(5, 1);
        break;
    }

    valor = valor - 1;
    count = count -1;
    save();
};

/**
 * @function criarTabela
 * @param {*} event 
 */
//criar tabela
function criarTabela(event){
    event.preventDefault();
    if(valor < MAX){ // Se valor for menor que constante MAX
    table = new tableCreate(valor); // então cria nova tabela
    table.init(); // inicializa tabela
    prog[valor - 1] = table;
    valor = valor + 1;
    count = count + 1;
    console.log(count);
    }
};

function updateTabela(){
  if(valor < MAX){ // Se valor for menor que constante MAX
  table = new tableCreate(valor); // então cria nova tabela
  table.init(); // inicializa tabela
  prog[valor - 1] = table;
  valor = valor + 1;
  console.log(count);
  }
};

function myFunction() {
    var x = document.getElementById("meuMenu");
    if (x.style.display === "block") {
      x.style.display = "none";
    } else {
      x.style.display = "block";
    }
  };


//Main



document.addEventListener("DOMContentLoaded", init, false);
btn_add.addEventListener('click', criarTabela);