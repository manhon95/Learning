void setupAP()
{
  WiFi.softAP(APssid, APpassword);             // Start the access point
  Serial.print("****** Access Point \"");
  Serial.print(APssid);
  Serial.println("\" started ******");
  delay(500);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("");
  display.println("WIFI AP O");
  display.display();
}


void setupSTA()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\n****** Connecting to ");
  Serial.print(ssid);
  Serial.println(" ******");
  display.print("WIFI STA ");
  display.display();
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) {
    delay(500);
    Serial.print(".");
  }
  if (i == 21) {
    Serial.print("Could not connect to");
    Serial.println(ssid);
    display.println("X");
    display.display();
  }
  else {
    Serial.print(" ");
    Serial.println("WiFi connected");
    Serial.println("");
    display.println("O");
    display.display();
    //start UART and the server
  }
}

void scanwifi(int i)
{
  serverClients[i].println("****** Scanning WIFI ******");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  if (n == 0)
    serverClients[i].println("****** No networks found *******");
  else
  {
    serverClients[i].print("****** ");
    serverClients[i].print(n);
    serverClients[i].println(" networks found ******");
    for (int j = 0; j < n; ++j)
    {
      // Print SSID and RSSI for each network found
      serverClients[i].print(j + 1);
      serverClients[i].print(": ");
      serverClients[i].print(WiFi.SSID(j));
      serverClients[i].print(" (");
      serverClients[i].print(WiFi.RSSI(j));
      serverClients[i].print(")");
      serverClients[i].println((WiFi.encryptionType(j) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  serverClients[i].println("");
}

void loginwifi(int i)
{ int ssidno;
  char password[30];
  int j = 0;
  int x;
  serverClients[i].print("WIFI no: ");
  while (serverClients[i].read() != 13) {
    delay(0);
  }
  /*while (serverClients[i].read() != 10) {
    delay(0);
  }*/
  ssidno = serverClients[i].parseInt();
  serverClients[i].print(ssidno);
  serverClients[i].print(": ");
  serverClients[i].print(WiFi.SSID(ssidno - 1));
  serverClients[i].print(" (");
  serverClients[i].print(WiFi.RSSI(ssidno - 1));
  serverClients[i].print(")");
  serverClients[i].println((WiFi.encryptionType(ssidno - 1) == ENC_TYPE_NONE) ? " " : "*");
  char ssid[WiFi.SSID(ssidno - 1).length() + 1];
  WiFi.SSID(ssidno - 1).toCharArray(ssid, WiFi.SSID(ssidno - 1).length() + 1);
  serverClients[i].print("debug: ");
  serverClients[i].println(ssid);
  
  if (WiFi.encryptionType(x - 1) == ENC_TYPE_NONE) {
    WiFi.begin(ssid);
  } else {
    serverClients[i].println("Please type password: ");
     password[0] = 0; 
     while (password[j] != 13) {
      password[j] = serverClients[i].read();
      j++;
      delay(0);
  }
    if (serverClients[i].read() == 10) {
    password[j] = 0;
    j = 0;
  }
    serverClients[i].print("debug: ");
    serverClients[i].println(password);
    WiFi.begin(ssid, password);
  }
  serverClients[i].print("****** Connecting to ");
  serverClients[i].print(ssid);
  serverClients[i].println(" ******");
  uint8_t k = 0;
  while (WiFi.status() != WL_CONNECTED && k++ < 20) {
    delay(500);
    serverClients[i].print(".");
  }
  if (k == 21) {
    serverClients[i].print("Could not connect to ");
    serverClients[i].println(WiFi.SSID(x - 1));
  }
  else {
    serverClients[i].println("WiFi connected");
    //start UART and the server
  }
}

void setupUDP() {
  Serial.println("Starting UDP");
  UDP.begin(123);                          // Start listening for UDP messages on port 123
  Serial.print("Local port:\t");
  Serial.println(UDP.localPort());
  Serial.println();
  display.println("UDP O");
  display.display();
}

void printBattery(int i)
{
  serverClients[i].print("****** ESP8266 Battery: ");
  serverClients[i].print(map(ESP.getVcc(), 0, 2960, 0, 100));
  serverClients[i].println("% ******");
  serverClients[i].println("");
}
