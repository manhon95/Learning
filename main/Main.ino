/*
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WiFi library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "SparkFunBME280.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define MAX_SRV_CLIENTS 10   //how many clients should be able to telnet to this ESP8266
#define rlen 50
#define OLED_DC     D9
#define OLED_CS     D10
#define OLED_RESET  D8

IPAddress timeServerIP;          // time.nist.gov NTP server address
const char* NTPServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message

byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

unsigned long intervalNTP = 60000; // Request NTP time every minute
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
uint32_t timeUNIX = 0;

unsigned long prevActualTime = 0;

ADC_MODE(ADC_VCC);

const char* ssid = "dlink-4532";
const char* password = "chansfhayes91";
const char* APssid = "Victor-ESP8266";
const char* APpassword = "90349929";

char reply[MAX_SRV_CLIENTS][rlen];
size_t clen[MAX_SRV_CLIENTS];

Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];
WiFiUDP UDP;                     // Create an instance of the WiFiUDP class to send and receive

void setup() {
  Serial.begin(115200);
  setupOLED();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Battery: ");
  display.print(map(ESP.getVcc(), 0, 2960, 0, 100));
  display.println("%");
  display.display();
  Serial.print("ESP8266 Battery: ");
  Serial.print(map(ESP.getVcc(), 0, 2960, 0, 100));
  Serial.println("%");
  delay(500);

  setup280();
  delay(500);
  setupSTA();
  delay(500);
  setupAP();
  display.clearDisplay();
  display.setCursor(0,0);
  delay(500);
  setupUDP();
  delay(500);
  setupNTP();
  delay(2000);
  display.clearDisplay();

  server.begin();
  server.setNoDelay(true);
  Serial.print("****** Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect. ******");
  Serial.println();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(4,4);
  display.print("TELNET ");
  display.display();
  delay(500);
  display.setTextColor(WHITE); // 'inverted' text
  display.println(WiFi.localIP());
  display.display();
  display.startscrollright(0x00, 0x0F);
  display.clearDisplay();  
}

void loop() {

  updateNTP();
  
  uint8_t i;
  uint8_t j;
  //check if there are any new clients

  if (server.hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial.print("New client: ");
        Serial.println(i + 1);
        for (j = 0; j < MAX_SRV_CLIENTS; j++) {
          serverClients[j].print("Client ");
          serverClients[j].println(i + 1);
          serverClients[j].println(" connected");
        }
        printBattery(i);
        serverClients[i].println("Type \"/batt\" to check battery");
        serverClients[i].println("     \"/hmt\" to check humidity");
        serverClients[i].println("     \"/prs\" to check pressure");
        serverClients[i].println("     \"/alt\" to check altitude");
        serverClients[i].println("     \"/tem\" to check temperature");
        serverClients[i].println("     \"/time\" to check time");
        break;
      }
    }
    //no free/disconnected spot so reject
    if (i == MAX_SRV_CLIENTS) {
      WiFiClient serverClient = server.available();
      serverClient.stop();
      Serial.println("Connection rejected ");
    }
  }
  //check clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      if (serverClients[i].available()) {
        //get data from the telnet client and push it to the UART
        while (serverClients[i].available()) {

          reply[i][clen[i]] = serverClients[i].read();

          if (reply[i][clen[i]] == 13) {
            if (serverClients[i].read() == 10) {
              //"Enter" = char(13)+char(10)
              reply[i][clen[i]] = 0;

              serverClients[i].print(">>>>>>");
              for (j = 0; j < MAX_SRV_CLIENTS; j++) {
                serverClients[j].print("Client ");
                serverClients[j].print(i + 1);
                serverClients[j].print(": ");
                serverClients[j].write(reply[i], clen[i]);
                serverClients[j].println("");
              }
              Serial.print("Client ");
              Serial.print(i + 1);
              Serial.print(": ");
              Serial.write(reply[i], clen[i]);
              Serial.println("");
              // print reply to serve and clients

              if (String(reply[i]) == "/batt") {
                printBattery(i);
              }
              if (String(reply[i]) == "/hmt") {
                printHumidity(i);
              }
              if (String(reply[i]) == "/prs") {
                printPressure(i);
              }
              if (String(reply[i]) == "/alt") {
                printAltitude(i);
              }
              if (String(reply[i]) == "/tem") {
                printTemperature(i);
              }
              if (String(reply[i]) == "/time") {
                printTime(i);
              }
              if (String(reply[i]) == "/scan") {
                scanwifi(i);
              }
              if (String(reply[i]) == "/login") {
                              loginwifi(i);
              }

              //command

              for (j = 0; j < clen[i]; j++) {
                reply[i][j] = 0;
              }
              clen[i] = 0;
              //reset reply[i]
            }
          }
          else {
            clen[i] = clen[i] + 1;
          }

        }
      }
    }
  }

  if (Serial.available()) {
    //check UART for data
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        serverClients[i].print("Server: ");
        serverClients[i].write(sbuf, len);
        serverClients[i].println("");
        delay(1);
      }
    }
    Serial.print(">>>>>>");
    Serial.print("Server: ");
    Serial.write(sbuf, len);
    Serial.println("");
  }

}
