/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
#include "ESP8266WiFi.h"
void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop() {

  if (Serial.available()) {
    //check UART for data
    size_t len = Serial.available();
    char sbuf[len];
    Serial.readBytes(sbuf, len);
    Serial.println(sbuf);
    if (String(sbuf) == "//scan") {
      Serial.println("scan start");

      // WiFi.scanNetworks will return the number of networks found
      int n = WiFi.scanNetworks();
      Serial.println("scan done");
      if (n == 0)
        Serial.println("no networks found");
      else
      {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
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
      Serial.println("");
    }
    if (String(sbuf) == "//login") {
      while (!Serial.available()) {
        yield();
      }
      int x = Serial.parseInt();
      Serial.print(x);
      Serial.print(": ");
      Serial.print(WiFi.SSID(x-1));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(x - 1));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(x - 1) == ENC_TYPE_NONE) ? " " : "*");
      char ssid[WiFi.SSID(x - 1).length() + 1];
      WiFi.SSID(x - 1).toCharArray(ssid, WiFi.SSID(x - 1).length() + 1);
      Serial.println(ssid);
      
      if (WiFi.encryptionType(x - 1) == ENC_TYPE_NONE) {
        WiFi.begin(ssid);
      } else {
        Serial.print("Please type password: ");
        while (!Serial.available()) {
          yield();
        }
        size_t pwlen = Serial.available();
        char password[pwlen];
        Serial.readBytes(password, pwlen);
        Serial.println(password);
        WiFi.begin(ssid, password);
      }
      Serial.print("****** Connecting to ");
      Serial.print(ssid);
      Serial.println(" ******");
      uint8_t j = 0;
      while (WiFi.status() != WL_CONNECTED && j++ < 20) {
        delay(500);
        Serial.print(".");
      }
      if (j == 21) {
        Serial.print("Could not connect to ");
        Serial.println(WiFi.SSID(x - 1));
      }
      else {
        Serial.println("WiFi connected");
        Serial.println("");
        //start UART and the server
      }
    }
    memset(sbuf, 0, sizeof sbuf);
  }
}*/
