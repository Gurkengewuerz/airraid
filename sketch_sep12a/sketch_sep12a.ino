/*
	Air Raid ESP Controller
	Niklas Schütrumpf (Gurkengewuerz)
	Mail: kontakt@mc8051.de
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <EEPROM.h>

#include "settings.h"

Servo esc;

ESP8266WebServer server(80);

int escPin = 2;
int minPulseRate = 1000;
int maxPulseRate = 2000;
int throttleChangeDelay = 50;
int maxThrottle = 60;

String warnType = "none";
int last_set = 0;
int counter = 0;
int escSpeed;

int abcCounter = 0;
int abcSleeper = 0;

void setup() {

  Serial.begin(112500);
  Serial.setTimeout(500);

  EEPROM.begin(512);

  delay(30);

  maxThrottle = EEPROM.read(0);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  WiFi.hostname("AirRaid");

  Serial.print("Setting soft-AP ... ");

  boolean result = WiFi.softAP("AIR_RAID", "BBK-Raid-v01");
  if (result == true) {
    Serial.println("Ready");
  } else {
    Serial.println("Failed!");
  }

  server.on("/", handleRoot);
  server.on("/warn", handleWarn);
  server.on("/speed", handleSpeed);
  server.on("/css", handleCSS);
  server.on("/grid", handleCSSGrid);

  server.onNotFound(handleNotFound);
  server.begin();

  esc.attach(escPin, minPulseRate, maxPulseRate);
  esc.write(0);
}

void loop() {
  server.handleClient();

  if (warnType != "none") {
    int curr_time = millis();

    if (warnType == "stop") {
      resetVals();
    }

    if (curr_time - last_set >= throttleChangeDelay) {
      if (warnType == "entwarnung") {
        if (counter < entwarnung_length) {
          escSpeed = normalizeVal(entwarnung[counter]);
          esc.write(escSpeed);
          last_set = curr_time;
          counter++;
        } else {
          resetVals();
        }
      } else if (warnType == "probe") {
        if (counter < probe_length) {
          escSpeed = normalizeVal(probe[counter]);
          esc.write(escSpeed);
          last_set = curr_time;
          counter++;
        } else {
          resetVals();
        }
      } else if (warnType == "warnung") {
        if (counter < warnung_length) {
          escSpeed = normalizeVal(warnung[counter]);
          esc.write(escSpeed);
          last_set = curr_time;
          counter++;
        } else {
          resetVals();
        }
      } else if (warnType == "abc") {
        if (abcCounter < 2) {
          if(abcSleeper == 0 || millis() >= abcSleeper) {
            if (counter < abc_length) {
              escSpeed = normalizeVal(abc[counter]);
              esc.write(escSpeed);
              last_set = curr_time;
              counter++;
            } else {
              abcSleeper = millis() + 30000;
              counter = 0;
              last_set = 0;
              abcCounter++;
            }
          }
        } else {
          resetVals();
        }
      }
    }
  }
}

void handleRoot() {
  String warnName = "Nicht gesetzt/Gestoppt";
  if(warnType == "warnung") warnName = "Warnung";
  else if(warnType == "abc") warnName = "ABC-Alarm";
  else if(warnType == "probe") warnName = "Probe-Alarm";
  else if(warnType == "entwarnung") warnName = "Entwarnung";
  
  String out = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Air Raid | mc8051.de</title><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='stylesheet' type='text/css' href='/grid'/><link rel='stylesheet' type='text/css' href='/css'/></head><body>"
               "<div class='container'><div class='row'><div class='col-6 center'><h1 style='margin-top: 0px;'>Air Raid Control</h1></div><div class='col-6' style='text-align: right;'><b>MAC-Adresse</b>: ";
  out += WiFi.macAddress();
  out += "<br/><b>Hostname</b>: ";
  out += "AirRaid";
  out += "<br/><b>Verb. Clients</b>: ";
  out += WiFi.softAPgetStationNum();
  out += "<br/></div></div><hr/><div class='row' style='margin-bottom: 35px'><div class='col-12'><span style='color: yellow'>&#x26A0</span> <b style='color:red'>Aktueller Warntype:</b> ";
  out += warnName;
  out += "</br></br><a class='button red' href='/warn?type=warnung'>Warnung</a><a class='button red' href='/warn?type=abc'>ABC-Alarm</a><a class='button blue' href='/warn?type=probe'>Probe</a><a class='button green' href='/warn?type=entwarnung'>Entwarnung</a><a class='button dark_blue' href='/warn?type=stop'>Stop</a><div style='margin-top: 35px'><div class='col-8' style='margin-top: 10px'><input class='input' placeholder='Maximale Geschwindigkeit' id='speed' type='number' min='0' max='180' value='";
  out += maxThrottle;
  out +=  "' /></div><div class='col-4' style='margin-right: 0px'><a class='button dark_blue' onclick='setSpeed()' href='#'>Setzen</a></div></div></div></div><hr/><div class='row'><div class='col-12'><i>Twitter: <a target='_blank' href='https://twitter.com/Gurkengewuerz' title='Twitter: @Gurkengewuerz'>@Gurkengewuerz</a> | <a target='_blank' href='http://mc8051.de' title='Website'>mc8051.de</a></i></div></div></div>"
          "<script>function setSpeed() {window.location = '/speed?val=' + document.getElementById('speed').value}</script>"
          "</body></html>";

  server.send(200, "text/html", out);
}

void handleWarn() {
  String message = "";

  if (server.arg("type") == "") {
    message = "Parameter Not Found";
  } else {
    message = "<html><head><meta http-equiv='refresh' content='0; URL=/'></head></html>";

    String arg = server.arg("type");
    if (arg == "stop" || arg == "probe" || arg == "warnung" || arg == "abc" || arg == "entwarnung") {
      warnType = arg;
    }
  }

  server.send(200, "text/html", message);
}

void handleSpeed() {
  String message = "";

  if (server.arg("val") == "") {
    message = "Parameter Not Found";
  } else {
    message = "<html><head><meta http-equiv='refresh' content='0; URL=/'></head></html>";

    String arg = server.arg("val");
    int val = arg.toInt();

    if (val > 180)
      val = 180;
    if (val < 0)
      val = 0;

    maxThrottle = val;
    EEPROM.write(0, val);
    EEPROM.commit();
  }

  server.send(200, "text/html", message);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleCSS() {
  server.send(200, "text/css", CSS_MAIN);
}
void handleCSSGrid() {
  server.send(200, "text/css", CSS_GRID);
}

int normalizeVal(int perc) {
  return map(perc, 0, 100, 0, maxThrottle);
}

void resetVals() {
  warnType = "none";
  esc.write(0);
  counter = 0;
  last_set = 0;
  abcCounter = 0;
  abcSleeper = 0;
  Serial.println("Reset....");
}

