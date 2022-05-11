const char sets_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 mobile Soft-AP</h2>
  <p>
    <form method='get' action='esetw'><label>SSID: </label><input name='esid' value='%QSID%' length=32><br><input name='pass' length=64><input type='submit'><br><br></form>
  </p>
</body>
</html>)rawliteral";



