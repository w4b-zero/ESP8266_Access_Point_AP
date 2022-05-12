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

IPAddress local_IP(192,168,188,1); // IP of the AP 
IPAddress gateway(192,168,188,1); // Gateway of the AP
IPAddress subnet(255,255,255,0); // Subnetmask of the AP

#define DHTPIN 5     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

#define SCREEN_SWITCH_TIME 3000 // 3000=3Sek. to switch the infos on the ssd1306-Screen

// 0 = boot log & errors
// 1 = boot log & errors + systemmessages
// 2 = boot log & errors + systemmessages + warnings
// 3 = boot log & errors + systemmessages + warnings + infos
int debug_mode = 1; 

// ********************
// *****Config end*****
// ********************


String PARAM_INPUT_1 = "esid";
String PARAM_INPUT_2 = "pass";
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
  return String();
}

void setup(){
  Serial.begin(115200);
  EEPROM.begin(512); 
  dht.begin();
  display.init();
  display.clear();

//  display.flipScreenVertically();
//  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 0, "Setting AP...");
  display.display(); 


  Serial.println();
  Serial.println("************************");
  Serial.println("*    mobile soft-ap    *");
  Serial.println("************************");
  Serial.println();
  Serial.println("boot: mobile soft-ap started....");
  Serial.println("boot: reading eeprom data");
  
  String qsid = "";
  String qsid2 ="";
  char* qsid_const = "";
  for (int i = 0; i < 32; ++i)
  {
    qsid += char(EEPROM.read(i));
    qsid_const += char(EEPROM.read(i));
    qsid2 += EEPROM.read(i);
  }
//  String qsidtest = "";
//  for (int itest = 0; itest < qsid.length(); ++itest)
// {
//    qsidtest += char(EEPROM.read(itest));
//  }


  if (debug_mode >= 3)
  {
//    Serial.print("info: SSIDtest: ");
//    Serial.println(qsidtest);
    Serial.print("info: SSID: ");
    Serial.println(qsid);

//    Serial.print("info: test: ");
//    Serial.println(qsid2);
  }
  if (qsid2 =="255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255"){
    if (debug_mode >= 1){
      Serial.println("system: SSID from eeprom is empty! - default used!");
    }
  } else if (qsid2 ==""){
    if (debug_mode >= 1){
      Serial.println("system: SSID from eeprom read error! - default used!");
    }
  } else {
    if (debug_mode >= 1){
      Serial.println("system: SSID from eeprom is used!");
    }

    esid = qsid;  // deaktivate for ignore eeprom data

    char* qsid_html = qsid_const;
     int count = 0;
 
    for (int i = 0; qsid_html[i]; i++)
        if (qsid_html[i] != ' ')
            qsid_html[count++] = qsid_html[i]; // here count is
                                   // incremented
    qsid_html[count] = '\0';
  }


  String qpass = "";
  String qpass2 = "";
  char* qpass_const = "";
  for (int i = 32; i < 96; ++i)
  {
    qpass += char(EEPROM.read(i));
    qpass_const += char(EEPROM.read(i));
    qpass2 += EEPROM.read(i);
  }
//  String qpasstest = "";
//  for (int itest = 32; itest < qpass.length(); ++itest)
//  {
//    qpasstest += char(EEPROM.read(itest));
//  }
  if (debug_mode >= 3)
  {
//    Serial.print("info: PASStest: ");
//    Serial.println(qpasstest);
    Serial.print("info: PASS: ");
    Serial.println(qpass);
//    Serial.print("info: test: ");
//    Serial.println(qpass2);
  }

  if (qpass2 =="255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255255"){
    if (debug_mode >= 1){
      Serial.println("system: PASS from eeprom is empty! - default used!");
    }
  } else if (qpass2 ==""){
    if (debug_mode >= 1){
      Serial.println("system: PASS from eeprom read error! - default used!");
    }
  } else {
    if (debug_mode >= 1){
      Serial.println("system: PASS from eeprom is used!");
    }
  
  pass = qpass;   // deaktivate for ignore eeprom data

    char* qpass_html = qpass_const;
     int count = 0;
 
    for (int i = 0; qpass_html[i]; i++)
        if (qpass_html[i] != ' ')
            qpass_html[count++] = qpass_html[i]; // here count is
                                   // incremented
    qpass_html[count] = '\0';
    }

  
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
    // GET input1 value on <ESP_IP>/update?output=<qsid>&state=<pass>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      esid = request->getParam(PARAM_INPUT_1)->value();
      pass = request->getParam(PARAM_INPUT_2)->value();
      //esid += ";";
      //pass += ";";
      digitalWrite(esid.toInt(), pass.toInt());
      if (esid.length() > 0 && pass.length() > 0) {
        if (debug_mode >= 1)
        {
          Serial.println("system: clearing eeprom");
        }
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        if (debug_mode >= 3)
        {
          Serial.print("info: esid:");
          Serial.println(esid);
          Serial.print("info: pass:");
          Serial.println(pass);
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
        EEPROM.commit();
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        if (debug_mode >= 1)
        {
          Serial.print("system: new ssid:");
          Serial.println(esid);
          Serial.print("system: new password:");
          Serial.println(pass);
        }
        request->send(200, "text/plain", content);
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
