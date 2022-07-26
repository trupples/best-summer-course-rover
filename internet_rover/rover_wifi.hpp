#include <WiFi.h>
#include <ESPmDNS.h>

// config
#define SSID "Nelu"
#define PASSWORD "kekkeroni"
#define MDNS_NAME "rover1"

void rover_wifi_setup() {
  Serial.print("Connecting to wifi");

  WiFi.setHostname(MDNS_NAME);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");

  if(!MDNS.begin(MDNS_NAME)) {
    Serial.println("mDNS fail");
  }

  Serial.print("Wifi OK! Local IP: ");
  Serial.println(WiFi.localIP());
}
