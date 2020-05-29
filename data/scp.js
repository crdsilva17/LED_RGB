

function trocaCor(){
    var r = document.getElementById("red").value;
    var g = document.getElementById("green").value;
    var b = document.getElementById("blue").value;
    var xh = new XMLHttpRequest();
    xh.open("GET","/update", true);
    xh.onreadystatechange = function() { // Chama a função quando o estado mudar.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            // Requisição finalizada. Faça o processamento aqui.
        }
    }
    xh.send("R="+r+"&G="+g+"&B="+b); 
    r = Number(r).toString(16);
    g = Number(g).toString(16);
    b = Number(b).toString(16);
document.getElementById("menu").style.backgroundColor = "#" + r + "f" + g + "f" + b + "f";
}




