#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include "Config.h"
#include "Output.h"
#include "Update.h"
#include "WiFi.h"

ESP8266WebServer server ( 80 );

const int led = 2;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

void handleRoot() {
  digitalWrite ( led, 0 );
//  Serial.println ( "Serving website root" );
  char temp[600];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 600,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Zimply Relayz</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <div class='main'>\
    <div class='menu' style='float:left'><ul><li><a href='/switch'>Switch Relays</a></li><li><a href='/reboot'>Reboot</a></li><li><a href='/reset'>Reset Wifi</a></li><li><a href='/update'>Update</a></li></ul></div>\
    <h1>Hello from Zimply!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
    </div>\
  </body>\
</html>",

    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
  digitalWrite ( led, 1 );
}

void handleNotFound( void ) {
  digitalWrite ( led, 0 );
//  Serial.println ( "Page not found!" );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( led, 1 );
}

void drawGraph( void ) {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int voltage = analogRead(SENSOR_IN) * (1 / 1023) * 3.3;
  int y = (voltage - 0.5264) / 0.020398;
  for (int x = 10; x < 390; x+= 10) {
    voltage = analogRead(SENSOR_IN) * (1 / 1023) * 3.3;
    int y2 = (voltage * 3.3 - 0.5264) / 0.020398;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}

void doSwitch( void ){
  digitalWrite ( led, 0 );
  String deviceNum = server.arg("device");
  String command = server.arg("comm");
  String state = "Unknown";
  
  if (deviceNum == "1"){
    if (command == "1"){
      digitalWrite(DEVICE1, 0);
      state = "On";
    } else if (command == "0"){
      digitalWrite(DEVICE1, 1);
      state = "Off";
    }
  } else if (deviceNum == "2"){
    if (command == "1"){
      digitalWrite(DEVICE2, 0);
      state = "On";
    } else if (command == "0"){
      digitalWrite(DEVICE2, 1);
      state = "Off";
    }
  }
  
  String message = "<html>\
  <head>\
    <meta http-equiv='refresh' content='30;url=/' />\
    <title>Zimply Relayz</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Switching Output</h1>\
    <p>Output";
  message += deviceNum;
  message += " is now ";
  message += state;
  message += "</></body></html>";
  server.send ( 200, "text/html", message);
  digitalWrite ( led, 1 );
}

void doRead( void ) {
  digitalWrite ( led, 0 );
  String type = server.arg("type");
  char str_voltage[8], str_humidity[8], str_temp[8];
  float voltage = analogRead(SENSOR_IN) * (3.81 / 1024);
  dtostrf(voltage, 3, 2, str_voltage);
  float humidity = (voltage - 0.5264) / 0.020398;
  dtostrf(humidity, 4, 2, str_humidity);
  char message[100];

  // Temperature readings
  float temp;
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  dtostrf(temp, 4, 2, str_temp);
  
  
  if (type == "raw"){
    sprintf (message, "Raw input:%d, Calculated voltage:%s, Humidity:%s, Temperature:%s", analogRead(SENSOR_IN), str_voltage, str_humidity, str_temp);
  } else if (type == "humidity"){
    sprintf (message, "%s", str_humidity);
  } else if (type == "temperature"){
    sprintf (message, "%s", str_temp);
  }
  server.send ( 200, "text/plain", message );
  digitalWrite ( led, 1 );
}

void doUpdate( void ){
  digitalWrite ( led, 0 );

  String message = "<html>\
  <head>\
    <meta http-equiv='refresh' content='30;url=/' />\
    <title>Zimply Relayz</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Checking for Updates!</h1>\
    <p>If available updates are found, they will be installed and the device will reboot.</>\
    <p>This will generally take about 30 seconds. This webpage should redirect to the Zimply Relayz main page when done.</>\
  </body>\
</html>";
  server.send ( 200, "text/html", message);
  checkUpdate();
  digitalWrite ( led, 1 );
}

void doReset( void ){
  digitalWrite ( led, 0 );
  String message = "<html>\
  <head>\
    <meta http-equiv='refresh' content='3;url=/' />\
    <title>Zimply LEDz</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Resetting Wifi settings!</h1>\
    <p>After reset, the device will set up an Access Point. You will need to connect to it and specify your new wifi settings.</>\
  </body>\
</html>";
  server.send ( 200, "text/html", message);
  resetWifiSettings();
  digitalWrite ( led, 1 );
}


void doReboot( void ){
  digitalWrite ( led, 0 );
  String message = "<html>\
  <head>\
    <meta http-equiv='refresh' content='3;url=/' />\
    <title>Zimply LEDz</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Rebooting!</h1>\
  </body>\
</html>";
  server.send ( 200, "text/html", message);
  digitalWrite ( led, 1 );
  ESP.reset();
}

void initHTTPServer ( void ) {
  pinMode ( led, OUTPUT );
  pinMode ( DEVICE1, OUTPUT );
  pinMode ( DEVICE2, OUTPUT );
  digitalWrite ( led, 1 );
  DS18B20.begin();
 
  if ( MDNS.begin ( "esp8266" ) ) {
  }

  server.on ( "/", handleRoot );
  server.on ( "/update", doUpdate );
  server.on ("/reset", doReset );
  server.on ("/reboot", doReboot );
  server.on ( "/test.svg", drawGraph );
  server.on ("/switch", doSwitch );
  server.on ("/read", doRead );
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  server.begin();
//  Serial.println ( "HTTP server started" );
}

void runHTTPServer ( void ) {
  server.handleClient();
}
