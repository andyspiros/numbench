function sendConf() {
    var xmlhttp = new XMLHttpRequest();
    
    // Handler
    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState==4 && xmlhttp.status==200) {
            var pre = document.createElement('pre');
            pre.innerHTML = xmlhttp.responseText;
            document.getElementsByTagName('body')[0].appendChild(pre);
        }
    }
    
    // Send request
    xmlhttp.open("POST", 'getXML', true);
    xmlhttp.setRequestHeader("Content-type",
                             "application/x-www-form-urlencoded");
    xmlhttp.send(generateRequestString());
}