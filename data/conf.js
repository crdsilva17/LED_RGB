
//Variaveis Globais
/**
 * @global
 */
var btn_add = document.querySelector("#btn-add");
var local = document.querySelector("#local");
var prog = [];
var hora = [];
var valor = 1;
var MAX = 6;


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
        var nova_tabela = document.querySelector("#template1");
        div = nova_tabela.content.querySelector("div");
        div.id = this.index;
        titulo = nova_tabela.content.querySelector("th");
        titulo.textContent = "Programação " + this.index;
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
            dadosJson = JSON.parse(this.responseText); // Recebe dados de atualização da página
            prog = dadosJson.pgr;
            hora = dadosJson.hr;
            
        }

};

//Função que envia valores para o servidor
function save(){


};

/**
 * @function delet
 * @param {*} t 
 */
//Deleta programas e atualiza dados no servidor
function delet(t){
    childEl = document.getElementById(t);
    parentEl = childEl.parentNode;
    parentEl.removeChild(childEl);
    for (t; t < valor-1; t++) {
        postEl = document.getElementById(t + 1);
        postEl.id = t;
        childEl =  postEl.lastElementChild;
        childEl.querySelector("th").textContent = "Programação " + t;
        btn = document.querySelector("input[id='b"+(t+1)+"']");
        btn.setAttribute('onclick', 'delet('+t+')');
        btn.id = "b"+t;
    }
    valor = valor - 1
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
    }
};


function myFunction() {
    var x = document.getElementById("meuMenu");
    if (x.style.display === "block") {
      x.style.display = "none";
    } else {
      x.style.display = "block";
    }
  }


//Main

btn_add.addEventListener('click', criarTabela);

document.addEventListener("DOMContentLoaded", init, false);