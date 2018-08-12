void setup280()
{
  Wire.begin();
  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("BME280 did not respond. Please check wiring.");
  }
  else
  Serial.println("BME280 ready");
}

void printHumidity(int i)
{
   serverClients[i].print("****** Humidity: ");
   serverClients[i].print(mySensor.readFloatHumidity(), 0);
   serverClients[i].println("%RH ******");
}

void printPressure(int i)
{
   serverClients[i].print("****** Pressure: ");
   serverClients[i].print(mySensor.readFloatPressure(), 0);
   serverClients[i].println("Pa ******");
}

void printAltitude(int i)
{
   serverClients[i].print("****** Altitude: ");
   serverClients[i].print(mySensor.readFloatAltitudeMeters(), 1);
   serverClients[i].println("m ******");
}

void printTemperature(int i)
{
   serverClients[i].print("****** Temperature: ");
   serverClients[i].print(mySensor.readTempC(), 1);
   serverClients[i].println("℃ ******");
}


