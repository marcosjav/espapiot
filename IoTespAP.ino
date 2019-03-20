/*
   starting from the WiFi example
   This program creates a WiFi AP for select and connect to a existing WiFi. Saving in EEPROM the selected SSID (32bytes) and the current pass (64bytes)
   Once is internet connection estabished, it will comunicate with the cloud server and create a local network server no do stuff
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "thereisnospoon"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";

/* webpage constants*/
const char *_htmlhead = "<!DOCTYPE html><html><head><meta charset='utf-8'></head><body>";
const char *_htmlfoot = "</body></html>";

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web Server
ESP8266WebServer server(80);

/* 
 *  Request mannagement
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

void handleNotFound(){
  String message = "<h1>No e encuentra la página</h1>";
  server.send(200, "text/html", _htmlhead + message + _htmlfoot);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  
  WiFi.softAP(ssid, password);

  //IPAddress myIP = WiFi.softAPIP();
  IPAddress ip(192, 168, 43, 1);
  IPAddress gateway(192, 168, 43, 1);
  IPAddress subnet(255, 255, 255, 0);
  if (WiFi.softAPConfig(ip, gateway, subnet))
    Serial.println("Ready");
  else
    Serial.println("Problem");

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", ip);

  handlingPath();

  server.begin();
  Serial.println("HTTP server started");
  scan();
}

void handlingPath(){
  server.on("/", handleRoot);
  server.on("/select", askPass);
  server.on("/connect", connectNet);
  server.onNotFound(handleNotFound);
}

void loop() {
  //DNS
  dnsServer.processNextRequest();
  //HTTP
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

/*
 * EEPROM EXAMPLE
 
#include <EEPROM.h>

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;

 void setup() {
  EEPROM.begin(512);
}

void loop() {
  // need to divide by 4 because analog inputs range from
  // 0 to 1023 and each byte of the EEPROM can only hold a
  // value from 0 to 255.
  int val = analogRead(A0) / 4;

  // write the value to the appropriate byte of the EEPROM.
  // these values will remain there when the board is
  // turned off.
  EEPROM.write(addr, val);

  // advance to the next address.  there are 512 bytes in
  // the EEPROM, so go back to 0 when we hit 512.
  // save all changes to the flash.
  addr = addr + 1;
  if (addr == 512) {
    addr = 0;
    EEPROM.commit();
  }

  delay(100);
}
 */

