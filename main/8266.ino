void setupAP()
{ 
  WiFi.softAP(APssid, APpassword);             // Start the access point
  Serial.print("****** Access Point \"");
  Serial.print(APssid);
  Serial.println("\" started ******");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("");
}


void setupSTA()
{ 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\n****** Connecting to "); 
  Serial.print(ssid);
  Serial.println(" ******"); 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20){
  delay(500);
  Serial.print(".");
  }
  if(i == 21){
    Serial.print("Could not connect to"); 
    Serial.println(ssid);
  }
  Serial.print(" ");
  Serial.println("WiFi connected");
  Serial.println("");
  //start UART and the server
}


void printBattery(int i)
{
   serverClients[i].print("****** ESP8266 Battery: ");
   serverClients[i].print(map(ESP.getVcc(), 0, 2960, 0, 100));
   serverClients[i].println("% ******");
   serverClients[i].println("");
}
