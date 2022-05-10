/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-nodemcu-access-point-ap-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  the original repository: https://github.com/RuiSantosdotme/Random-Nerd-Tutorials
  the original Sketch: https://github.com/RuiSantosdotme/Random-Nerd-Tutorials/blob/master/Projects/ESP8266/ESP8266_Access_Point_AP.ino
  
**********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>               
#include "SSD1306Wire.h"
// Import Wifi Logo         
#include "data/images.h"

// *****Config*******
const char* ssid     = "Test-AP"; // Name of the AP
const char* password = "123456789"; // Password of the AP

IPAddress local_IP(192,168,188,1); // IP of the AP 
IPAddress gateway(192,168,188,1); // Gateway of the AP
IPAddress subnet(255,255,255,0); // Subnetmask of the AP

#define DHTPIN 5     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

#define SCREEN_SWITCH_TIME 3000 // 3000=3Sek. to switch the infos on the ssd1306-Screen

// *****Config end*****


typedef void (*Demo)(void);
int demoMode = 0;
DHT dht(DHTPIN, DHTTYPE);
SSD1306Wire display(0x3c, 4, 14);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

const char index_html[] PROGMEM = R"rawliteral(
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
  <h2>ESP8266 DHT Server</h2>
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

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  display.init();
  display.clear();

  //display.flipScreenVertically();
//  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 0, "Setting AP...");
  display.display(); 
  //delay(1000);

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
      Serial.print("SSID: ");
      Serial.println(ssid);

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  
  IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  display.clear();

  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // Start server
  server.begin();
}
 
void starttext() {
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, "Mobile AP");
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 15, "SSID:");
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 24, ssid);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 49, "Active");
}


void statstext() {
  IPAddress IP = WiFi.softAPIP();
  String s="";
  for (int i=0; i<4; i++) {
    s += i  ? "." + String(IP[i]) : String(IP[i]);
  }
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 0, "Stats");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 25, "Clients:");
  display.drawString(55, 25, String(WiFi.softAPgetStationNum()));
  display.drawString(0, 49, "IP:");
  display.drawString(17, 49, s);
}

void logo() {
  display.drawXbm(34, 13, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 49, ssid);
}
Demo demos[] = {starttext, statstext, logo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void loop(){
  // clear the display
  display.clear();
  // draw the current demo method


  if (millis() - timeSinceLastModeSwitch > SCREEN_SWITCH_TIME) {
    demoMode = (demoMode + 1)  % demoLength;
  demos[demoMode]();
    display.display();
      Serial.println("info: switch screen");

    timeSinceLastModeSwitch = millis();
  }


  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println("temp: " + String(t));
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println("humi:" + String(h));
    }
      Serial.print("Clients:");
      Serial.println(String(WiFi.softAPgetStationNum()));

  }

}
