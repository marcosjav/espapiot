/*
   starting from the WiFi example
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "thereisnospoon"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
const char *_htmlhead = "<!DOCTYPE html><html><head><meta charset='utf-8'></head><body>";
const char *_htmlfoot = "</body></html>";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  server.send(200, "text/html", _htmlhead + showHTTP() + _htmlfoot);
}
void askPass() {
  String message = "<h1>Seleccionó la red <b>";
  int i = server.arg(0).toInt();
  message += WiFi.SSID(i);
  message += "</b></h1>";
  message += "<form action='/connect'>Contraseña:<input name='password' type='password'><br><br><input type='submit' value='Conectar'></form>";
  server.send(200, "text/html", _htmlhead + message + _htmlfoot);
}

void connectNet(){
  String message = "PENDIENTE<br/>";
  for(int i = 0; i < server.args(); i++){
    message += server.argName(i) + ":";
    message += server.arg(i) + "<br/>";
  }
  server.send(200, "text/html", _htmlhead + message + _htmlfoot);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  //IPAddress myIP = WiFi.softAPIP();
  IPAddress ip(192, 168, 43, 1);
  IPAddress gateway(192, 168, 43, 1);
  IPAddress subnet(255, 255, 255, 0);
  if (WiFi.softAPConfig(ip, gateway, subnet))
    Serial.println("Ready");
  else
    Serial.println("Problem");

  handlingPath();

  server.begin();
  Serial.println("HTTP server started");
  scan();
}

void handlingPath(){
  server.on("/", handleRoot);
  server.on("/select", askPass);
  server.on("/connect", connectNet);
}

void loop() {
  server.handleClient();
}

int scan(){
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  return n;
}

String showHTTP(){
  int n = scan();
  String html = "<h1>";

   if (n == 0) {
    html += "No se escontraron redes</h1>";
  } else {
    html += n;
    html += " redes encontradas</h1><h2>Seleccione:</h2>";
    for (int i = 0; i < n; ++i) {
      html += "<a href='/select?id=";
      html += i;
      html += "'>" + WiFi.SSID(i);
      html += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
      html += "</a><br/>";
    }
  }
  Serial.println(html);
  return html;
}



