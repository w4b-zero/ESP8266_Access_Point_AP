const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style type="text/css">
body {
    font-family: Verdana, Arial, Helvetica, sans-serif;
    font-size: 13px;
    color:#000000;
    background: #333333;
}

p {
    /*padding: 10px;*/
}

#wrapper {
    width: 100%%;
    min-width: 300px;
    max-width: 1024px;
    margin: 0 auto;
}

#headerwrap {
    width: 100%%;
    float: left;
    margin: 0 auto;
}

#header {
    height: 75px;
    background: #FF6633;
    border-radius: 10px;
    border: 1px solid #ffffff;
    margin: 5px;
    text-align: center;
}

#navigationwrap {
    width: 100%%;
    float: left;
    margin: 0 auto;
}

#navigation {
    height: 40px;
    background: #999999;
    border-radius: 10px;
    border: 1px solid #ffffff;
    margin: 5px;
    text-align: center;
    padding: 5px 5px 5px 5px;
}

#contentliquid {
    float: left;
    width: 100%%;
}

#contentwrap {
    width: 100%%;
    float: left;
}

#content {
    background: #cccccc;
    border-radius: 10px;
    border: 1px solid #ffffff;
    margin: 5px;
    text-align: center;
    padding: 10px 10px 10px 10px;
    display: flex;
    justify-content: center;
}

#tabellewrap {
    float: left;
    width: 100%%;
    background: #e6e5e5;
    border-radius: 10px;
    border: 1px solid #000000;
    margin: 5px;
    padding: 5px 5px 5px 5px;
}

#tabelle_left {
    background: #e6e5e5;
    border-radius: 10px;
    border: 1px solid #000000;
    margin: 5px;
    padding: 5px 5px 5px 5px;
    display: flex;
    justify-content: left;
    text-align: left;
    width: 100%%;
}
#tabelle_center {
    display: flex;
    justify-content: center;
    text-align: center;
    width: 100%%;
}
#tabelle_header {
    background: #e6e5e5;
    border-radius: 10px;
    border: 1px solid #000000;
    margin: 0px 0px 0px 5px;
    padding: 5px 5px 5px 5px;
    display: flex;
    justify-content: center;
    text-align: center;
    width: 100%%;
}
#tabelle_right {
    background: #e6e5e5;
    border-radius: 10px;
    border: 1px solid #000000;
    margin: 5px;
    padding: 5px 5px 5px 5px;
    display: flex;
    justify-content: right;
    text-align: right;
    width: 50%%;
}

#footerwrap {
    width: 100%%;
    float: left;
    margin: 0 auto;
    clear: both;
}

#footer {
    height: 40px;
    background: #ff6633;
    border-radius: 10px;
    border: 1px solid #ffffff;
    margin: 5px;
    padding: 0px 10px 10px 0px;
    text-align: right;
    font-size: 15px;
}

#link_button {
    background: #ff6633;
    border-radius: 10px;
    border: 1px solid #000000;
    margin: 5px;
    padding: 10px 10px 10px 10px;
    text-align: center;
}
      /* unvisited link */
      a:link {
        color: rgb(0, 0, 0);
      }
      /* visited link */
      a:visited {
        color: #000000;
      }
      /* mouse over link */
      a:hover {
        color: #0000ff;
      }
      /* selected link */
      a:active {
        color: #ffff00;
      }

</style>
</head>
<body>
  <div id="wrapper">
    <div id="headerwrap">
      <div id="header">
        <h1>ESP8266 mobile Soft-AP</h1>
      </div>
    </div>
    <div id="navigationwrap">
      <div id="navigation">
        <p><a id="link_button" href='/'>Home</a><a id="link_button" href='/sets'>Settings</a></p>
      </div>
    </div>
    <div id="contentliquid"><div id="contentwrap">
      <div id="content">
  <p>
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
      </div>
    </div>
  </div>
  <div id="footerwrap">
    <div id="footer">
      <p>code from <a href="https://github.com/RuiSantosdotme" target="_blank">Rui Santos</a> modify by <a href="https://github.com/w4b-zero" target="_blank">zero&trade;</a> v. 0.0.1</p>
    </div>
  </div>
</div>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";
