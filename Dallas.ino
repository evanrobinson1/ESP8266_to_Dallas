// ESP8266 NodeMCU displaying Dallas DS18S20 temperature sensor readings on a Webpage by Evan Robinson

// Import required libraries

#include <DallasTemperature.h>
#include <OneWire.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>


// -------- One Wire stuff -----------
#define ONE_WIRE_BUS 4                          //Raw pin 4 is D2 pin of nodemcu
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);            // Pass the oneWire reference to Dallas Temperature.

float tf = 0.0; // variable for current temperature updated in loop()

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 20 seconds
const long interval = 20000;  

const char index_html[] PROGMEM = R"rawliteral(         
<!DOCTYPE HTML><html>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<HTML>
<HEAD>
<TITLE>Dallas Temperature</TITLE>

<META content="IE=5.0000" http-equiv="X-UA-Compatible">
<META content="text/html; charset=iso-8859-1" http-equiv=Content-Type>
<meta http-equiv="refresh" content="30">

</HEAD>

<BODY bgColor=#ffffff text=#000000>

<P><FONT size=6><FONT color=blue>Dallas Temperature</FONT></P>

<TABLE border=1 width=250>  
<TBODY>
<TR>   
<TD height=200 width=300 style="text-align:center">

    <span id="tf">%TF%</span>
    <sup class="units">&deg;</sup>

</TD></TR></TABLE>

<SCRIPT>

// server functions:

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("tf").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/tf", true);
  xhttp.send();
}, 10000 ) ;

</SCRIPT>)rawliteral";    // end webpage


// Replaces placeholders with values

  String processor(const String& var){
  Serial.println(var);
 
  if (var == "TF"){
    return String(tf);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // ds18s20
  sensors.begin();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
 
 server.on("/tf", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(tf).c_str());
  });
  
  // Start server
  server.begin();  
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

// Dallas:
sensors.requestTemperatures();                // Send the command to get temperatures
  tf = sensors.getTempFByIndex(0);
  Serial.println("Temperature is: ");
  Serial.println(tf);
  delay(500);
}
}