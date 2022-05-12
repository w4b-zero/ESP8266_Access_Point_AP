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
  </style>
</head>
<body>
  <h2>ESP8266 mobile Soft-AP</h2>
    <a href='/'>back</a>

    <form method='get' action='esetw'>
      <table border='1'>
        <tr>
          <td colspan='2' align='center'>Soft-AP Zugangsdaten</td>
        </tr>
        <tr>
          <td align='right'><label>SSID: </label></td><td><input name='esid' value='%QSID%' length=32></td>
        </tr>
        <tr>
          <td align='right'>Passwort:</td><td><input name='pass' value='%QPASS%' length=64></td>
        </tr>
        <tr>
          <td colspan='2' align='center'><input type='submit'></td>
       </tr>
      </table>
    </form>
</body>
</html>)rawliteral";



