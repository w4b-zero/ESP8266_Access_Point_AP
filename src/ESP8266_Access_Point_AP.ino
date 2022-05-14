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
#include <EEPROM.h>
#include <FastLED.h>

// Import Wifi Logo         
#include "data/images.h"

// Import HTML-Pages
#include "html/index.h"
#include "html/dhtinfo.h"
#include "html/sets.h"

// ******************
// *****Config*******
// ******************
String esid = "Test-AP"; // Name of the AP
String pass = "123456789"; // Password of the AP
String apip = "192.168.1.1"; // IP of the AP

#define DHTPIN 5     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

#define NUM_LEDS 1
#define DATA_PIN 12
#define CLOCK_PIN 13


#define SCREEN_SWITCH_TIME 3000 // 3000=3Sek. to switch the infos on the ssd1306-Screen

// 0 = boot log & errors
// 1 = boot log & errors + systemmessages
// 2 = boot log & errors + systemmessages + warnings
// 3 = boot log & errors + systemmessages + warnings + infos
// 4 = boot log & errors + systemmessages + warnings + infos + debug
int debug_mode = 3; 

// ********************
// *****Config end*****
// ********************
CRGB leds[NUM_LEDS];

String firmware_version = "v0.0.2";

String PARAM_INPUT_1 = "esid";
String PARAM_INPUT_2 = "pass";
String PARAM_INPUT_3 = "apip";
String content;
int i = 0;
int statusCode;

typedef void (*Demo)(void);
int demoMode = 0;
DHT dht(DHTPIN, DHTTYPE);
SSD1306Wire display(0x3c, 4, 14);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
String qsid_html = esid;
String qpass_html = pass;
String qapip_html = apip;
String qclients_html = "0";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 30 seconds
const long interval = 30000;  



// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "QSID"){
    return String(qsid_html);
  }
  else if(var == "QPASS"){
    return String(qpass_html);
  }
  else if(var == "QAPIP"){
    return String(qapip_html);
  }
  else if(var == "QCLIENTS"){
    return String(qclients_html);
  }
  else if(var == "FIRMWARE_VERSION"){
    return String(firmware_version);
  }
  return String();
}

String edata(int start, int ende){
  String qsid = "";
  String qsid3 ="";
  for (int i = start; i < ende; ++i)
  {
    qsid3 = EEPROM.read(i); // 1 char to 1 string
    int count = 0;
    if (qsid3 == String('0')){} 
    else {
      qsid += char(EEPROM.read(i)); // here count is
      if (debug_mode >= 4)
      {
        Serial.print("char: ");
        Serial.println(qsid3);
      }
    }
  }
  return qsid;
}

void setup(){
  Serial.begin(115200);
  EEPROM.begin(512); 
  dht.begin();
  display.init();
  display.clear();
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
LEDS.setBrightness(20);
//  display.flipScreenVertically();
//  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 0, "Setting AP...");
  display.display(); 
    leds[0].setRGB( 255, 0, 0);
    FastLED.show();



  Serial.println();
  Serial.println("************************");
  Serial.println("*    mobile soft-ap    *");
  Serial.println("************************");
  Serial.println();
  Serial.println("boot: mobile soft-ap started....");
  Serial.println("boot: reading eeprom data");
  delay(1000);

  String edata_ssid = edata(0,32);
  String edata_pass = edata(32,96);

  if (debug_mode >= 3)
  {
    Serial.print("edata SSID: ");
    Serial.println(edata_ssid);
    Serial.print("edata PASS: ");
    Serial.println(edata_pass);
  }
  if (edata_ssid =="" && edata_pass ==""){
    if (debug_mode >= 1){
      Serial.println("system: Data from eeprom is empty! - default used!");
    }
  } else {
    if (debug_mode >= 1){
      Serial.println("system: Data from eeprom is used!");
    }
    String qsid_html = edata_ssid;
    esid = edata_ssid;  // deaktivate for ignore eeprom data
    String qpass_html = edata_pass;
    pass = edata_pass;  // deaktivate for ignore eeprom data
  }

  String edata_ip = edata(96,111);
 if (debug_mode >= 3)
  {
    Serial.print("edata IP: ");
    Serial.println(edata_ip);
  }
  if (edata_ip ==""){
    if (debug_mode >= 1){
      Serial.println("system: AP-IP from eeprom read error! - default used!");
    }
  } else {
    if (debug_mode >= 1){
      Serial.println("system: AP-IP from eeprom is used!");
    }
    String qapip_html = edata_ip;
    apip = edata_ip;  // deaktivate for ignore eeprom data
  }
  
  IPAddress ap_ip;
  const char *ap_ipch;
  ap_ipch = apip.c_str();
  ap_ip.fromString(ap_ipch);

  if (debug_mode >= 4)
  {
    Serial.print("apip: ");
    Serial.println(apip);
    Serial.print("parse: ");
    Serial.println(ap_ip[0]);
    Serial.print("parse: ");
    Serial.println(ap_ip[1]);        
    Serial.print("parse: ");
    Serial.println(ap_ip[2]);
    Serial.print("parse: ");
    Serial.println(ap_ip[3]);
  }
  IPAddress local_IP(ap_ip[0],ap_ip[1],ap_ip[2],ap_ip[3]); // nutze ip aus eeprom 
  IPAddress gateway(ap_ip[0],ap_ip[1],ap_ip[2],ap_ip[3]); // nutze gareway aus eeprom
  IPAddress subnet(255,255,255,0); // Subnetmask of the AP

      leds[0].setRGB( 255, 255, 0);
      FastLED.show();
  delay(1000);

  Serial.print("boot: setting soft-ap configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("boot: start soft-ap server ... ");
  Serial.println(WiFi.softAP(esid.c_str(), pass.c_str()) ? "Ready" : "Failed!");
  Serial.print("boot: ssid:");
  Serial.println(esid.c_str());
  Serial.print("boot: soft-ap ip:");
  Serial.println(WiFi.softAPIP());

  Serial.println("boot: soft-ap startet!");
  Serial.println();
  Serial.println("************************");
  Serial.println("*mobile soft-ap online!*");
  Serial.println("************************");
  Serial.println();

  display.clear();
      leds[0].setRGB( 0, 255, 0);
      FastLED.show();
  delay(1000);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/dht", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", dhtinfo_html, processor);
  });
  server.on("/sets", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", sets_html, processor);
  });
  server.on("/esetw", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String esid;
    String pass;
    String apip;
    // GET input1 value on <ESP_IP>/update?output=<qsid>&state=<pass>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_2)) {
      esid = request->getParam(PARAM_INPUT_1)->value();
      pass = request->getParam(PARAM_INPUT_2)->value();
      apip = request->getParam(PARAM_INPUT_3)->value();
      //esid += ";";
      //pass += ";";
      digitalWrite(esid.toInt(), pass.toInt());
      if (esid.length() > 0 && pass.length() > 0) {
        if (debug_mode >= 1)
        {
          Serial.println("system: clearing eeprom");
        }
        for (int i = 0; i < 111; ++i) {
          EEPROM.write(i, 0);
        }
        if (debug_mode >= 3)
        {
          Serial.print("info: esid:");
          Serial.println(esid);
          Serial.print("info: pass:");
          Serial.println(pass);
          Serial.print("info: apip:");
          Serial.println(apip);
        }
        if (debug_mode >= 1)
        {
          Serial.println("system: writing eeprom ssid:");
        }
        for (int i = 0; i < esid.length(); ++i)
        {
          EEPROM.write(i, esid[i]);
          if (debug_mode >= 1)
          {
            Serial.print("system: wrote: ");
            Serial.println(esid[i]);
          }
        }
        if (debug_mode >= 1)
        {
          Serial.println("writing eeprom pass:");
        }
        for (int i = 0; i < pass.length(); ++i)
        {
          EEPROM.write(32 + i, pass[i]);
          if (debug_mode >= 1)
          {
            Serial.print("system: wrote: ");
            Serial.println(pass[i]);
          }
        }
        if (debug_mode >= 1)
        {
          Serial.println("writing eeprom apip:");
        }
        for (int i = 0; i < apip.length(); ++i)
        {
          EEPROM.write(96 + i, apip[i]);
          if (debug_mode >= 1)
          {
            Serial.print("system: wrote: ");
            Serial.println(apip[i]);
          }
        }
        EEPROM.commit();
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        if (debug_mode >= 1)
        {
          Serial.print("system: new ssid:");
          Serial.println(esid);
          Serial.print("system: new password:");
          Serial.println(pass);
          Serial.print("system: new ip:");
          Serial.println(apip);
        }
        request->send(200, "text/plain", content);
        delay(5000);
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        if (debug_mode >= 0)
        {
         Serial.println("error: empty data send!");
        }
      }
    }
    else {
      content = "{\"Error\":\"No Data send!\"}";
      statusCode = 404;
      if (debug_mode >= 0)
      {
       Serial.println("error: no data send!");
      }
    }
    request->send(statusCode, "application/json", content);
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
  display.drawString(64, 24, esid);
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
  display.drawString(64, 49, esid);
}
Demo demos[] = {starttext, statstext, logo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void loop(){
  qsid_html = esid;
  qpass_html = pass;
  qapip_html = apip;
  String qclients =  String(WiFi.softAPgetStationNum());
  qclients_html = qclients;
  // clear the display
  display.clear();
  // draw the current demo method


  if (millis() - timeSinceLastModeSwitch > SCREEN_SWITCH_TIME) {
    demoMode = (demoMode + 1)  % demoLength;
    demos[demoMode]();
    display.display();
      if (debug_mode >= 3 )
      {
        Serial.println("info: switch screen");
      }
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
      if (debug_mode >= 0)
      {
        Serial.println("error: failed to read data from sensor!");
      }
    }
    else {
      t = newT;
      if (debug_mode >= 1)
      {
        Serial.println("system: temperature: " + String(t));
      }
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      if (debug_mode >= 0)
      {
        Serial.println("error: failed to read data from sensor!");
      }
    }
    else {
      h = newH;
      if (debug_mode >= 1)
      {
        Serial.println("system: humidity:" + String(h));
      }
    }
      if (debug_mode >= 1)
      {
        Serial.print("system: clients:");
        Serial.println(String(WiFi.softAPgetStationNum()));
      }

  }

}
