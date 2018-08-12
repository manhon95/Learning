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

#include <Wire.h>
#include "SparkFunBME280.h"
#include <ESP8266WiFi.h>

#define MAX_SRV_CLIENTS 10   //how many clients should be able to telnet to this ESP8266
#define rlen 50

ADC_MODE(ADC_VCC);

const char* ssid = "dlink-4532";
const char* password = "chansfhayes91";
const char* APssid = "Victor-ESP8266";
const char* APpassword = "90349929";

char reply[MAX_SRV_CLIENTS][rlen];
size_t clen[MAX_SRV_CLIENTS];


BME280 mySensor;
WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  Serial.begin(115200);
  Serial.print("ESP8266 Battery: ");
  Serial.print(map(ESP.getVcc(), 0, 2960, 0, 100));
  Serial.println("%");
  delay(500);

  setup280();
  delay(500);
  setupSTA();
  delay(500);
  setupAP();
  delay(500);

  server.begin();
  server.setNoDelay(true);
  Serial.print("****** Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect. ******");
  Serial.println();

}

void loop() {
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
  //check UART for data
  if (Serial.available()) {
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
