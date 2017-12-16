#include <DRV8835MotorShield.h>

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <math.h>

//#include "Adafruit_VL53L0X.h"

/* Set these to your desired credentials. */
const char *ssid = "ESPap";

#define left     15
#define right  12
#define backl     13
#define backr  14

#define USE_SERIAL Serial

//Adafruit_VL53L0X lox = Adafruit_VL53L0X();

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

float p = 0;
float d = 50;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
          //  USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            if(payload[0] == 'r') { 
                
                // decode rgb data
                 p = (uint32_t) strtol((const char *) &payload[1], NULL, 10);
    //Serial.println(r);

               
            }

            if(payload[0] == 'l') {
                
                // decode rgb data
                d = (uint32_t) strtol((const char *) &payload[1], NULL, 10);
    //Serial.println(l);
    //Serial.print(analogRead(LED_BLUE));
    

                
            }
            if(d>50){
            analogWrite(left,  int(p*(90/d-.8)));
            analogWrite(right,  p);
            }
            else if(d<=50){
            analogWrite(left,  p);
            analogWrite(right,  int(p*(d/50)));};
            
                
          /* if (p==20){
            spin();
            }
            */
            break;
            
    }

}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");

  /*if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  */
    //USE_SERIAL.begin(921600);
  //  USE_SERIAL.begin(115200);

    //USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    pinMode(left, OUTPUT);
    pinMode(right, OUTPUT);
    pinMode(backl, OUTPUT);
    pinMode(backr, OUTPUT);
   
digitalWrite(backl,  0);
            digitalWrite(backr, 0);
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid);
  
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        USE_SERIAL.println("MDNS responder started");
    }

    // handle index
    server.on("/", []() {
        // send index.html
        server.send(200, "text/html", 
        "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
    });

    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);


    

}

void loop() {
    /*VL53L0X_RangingMeasurementData_t measure;
     Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
  */
  webSocket.loop();
    server.handleClient();
   
     
}

void spin(){
            analogWrite(left,  1000);
            analogWrite(right, 0);
            
            delay(1000);

            analogWrite(left,  0);
            analogWrite(right, 0);

           p=0;

}

