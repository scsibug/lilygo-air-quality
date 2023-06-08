

  /*
  Reading CO2, humidity and temperature from the SCD4x
  By: Paul Clark
  Based on earlier code by: Nathan Seidle
  SparkFun Electronics
  Date: June 3rd, 2021
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/18365

  This example prints the current CO2 level, relative humidity, and temperature in C.

  Hardware Connections:
  Attach RedBoard to computer using a USB cable.
  Connect SCD40/41 to RedBoard using Qwiic cable.
  Open Serial Monitor at 115200 baud.
*/

#include <Wire.h>

#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
#include "SparkFun_ENS160.h"

// SCD4x
SCD4x scdSensor;
#define I2C_SDA 43
#define I2C_SCL 44

// ENS160
SparkFun_ENS160 ensSensor;
int ensStatus; 

void setup()
{
  Serial.begin(115200);
  Serial.println(F("SCD4x Example"));
  Wire.begin(I2C_SDA, I2C_SCL);

  //.begin will start periodic measurements for us (see the later examples for details on how to override this)
  if (scdSensor.begin() == false)
  {
    Serial.println(F("SCD Sensor not detected. Please check wiring. Freezing..."));
    while (1)
      ;
  }
  // configure SCD4x altitude for calibration
  scdSensor.setSensorAltitude(140);
  //The SCD4x has data ready every five seconds

  if (ensSensor.begin() == false)
  {
    Serial.println(F("ENS Sensor not detected. Please check wiring. Freezing..."));
    while (1)
      ;
  }
  // Reset the indoor air quality sensor's settings.
	if( ensSensor.setOperatingMode(SFE_ENS160_RESET) )
		Serial.println("ENS Now Ready.");
	delay(100);
  // Set to standard operation
  ensSensor.setOperatingMode(SFE_ENS160_STANDARD);
  ensStatus = ensSensor.getFlags();
	Serial.print("Gas Sensor Status Flag: ");
	Serial.println(ensStatus);
  Serial.println("Finished ENS sensor init");
  Serial.println("Exiting Setup");
}

void scdLoop() {
  if (scdSensor.readMeasurement()) // readMeasurement will return true when fresh data is available
  {
    Serial.print(F("CO2(ppm):"));
    Serial.print(scdSensor.getCO2());

    Serial.print(F("\tTemperature(C):"));
    Serial.print(scdSensor.getTemperature(), 1);

    Serial.print(F("\tHumidity(%RH):"));
    Serial.print(scdSensor.getHumidity(), 1);

    Serial.println();
  }
}

void ensLoop()
{
  // TODO:
  // Give values to Air Quality Sensor to improve accuracy
	//myENS.setTempCompensationCelsius(tempC); 
	//myENS.setRHCompensationFloat(rh); 
	if( ensSensor.checkDataStatus() )
	{
		Serial.print("Air Quality Index (1-5) : ");
		Serial.println(ensSensor.getAQI());

		Serial.print("Total Volatile Organic Compounds: ");
		Serial.print(ensSensor.getTVOC());
		Serial.println("ppb");

		Serial.print("CO2 concentration: ");
		Serial.print(ensSensor.getECO2());
		Serial.println("ppm");
	}
}


void loop()
{
 scdLoop();
 ensLoop();
 delay(500);
}