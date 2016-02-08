// Include Project files

#include "Output.h"
#include "HTTPServer.h"
#include "WiFi.h"

void setup() {
  initWiFi();
  initHTTPServer();
 
}

void loop() {
  // put your main code here, to run repeatedly:
  runHTTPServer();
  yield();
}
