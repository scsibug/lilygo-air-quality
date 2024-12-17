// sensors
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
#include "SparkFun_ENS160.h"

// screen
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

// I2C address for ESP32
#define I2C_SDA 43
#define I2C_SCL 44

// screen setup
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
// SCD4x
SCD4x scdSensor;
// ENS160
SparkFun_ENS160 ensSensor;
int ensStatus;

// Variables for Display
// high quality CO2
int hq_co2_ppm;
float temp_c;
float humidity_rh_perc;
int voc_ppb;
int aqi;

void setup() {
  Serial.begin(115200);
  Serial.println(F("SCD4x/ENS160 Air Quality Sensor"));
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.println("About to init TFT");
  tft.init();
  Serial.println("About to set rotation");
  tft.setRotation(1);
  // cycle through some colors on startup
  tft.fillScreen(TFT_BLACK);
  tft.fillScreen(TFT_RED); delay(100);
  tft.fillScreen(TFT_GREEN); delay(100);
  tft.fillScreen(TFT_BLUE); delay(100);
  tft.fillScreen(TFT_BLACK); delay(100);

  //.begin will start periodic measurements
  if (scdSensor.begin() == false) {
    Serial.println(F("SCD Sensor not detected. Please check wiring. Freezing..."));
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("SCD Sensor was not detected!", 5, 50, 2);
    while (1);
  }
  // configure SCD4x altitude for calibration.  Change this for your own altitude!
  scdSensor.setSensorAltitude(140);
  //The SCD4x has data ready every five seconds
  if (ensSensor.begin() == false) {
    Serial.println(F("ENS Sensor not detected. Please check wiring. Freezing..."));
    tft.drawString("ENS Sensor was not detected!", 5, 50, 2);
    while (1);
  }
  // Reset the indoor air quality sensor's settings.
  if (ensSensor.setOperatingMode(SFE_ENS160_RESET)) {
    Serial.println("ENS Now Ready.");
  }
  delay(100);
  // Set to standard operation
  ensSensor.setOperatingMode(SFE_ENS160_STANDARD);
  ensStatus = ensSensor.getFlags();
  Serial.print("Gas Sensor Status Flag: ");
  Serial.println(ensStatus);
  Serial.println("Finished ENS sensor init");
  Serial.println("Exiting Setup");
}

// Read data points from SCD4x sensor
void scdIter() {
  // readMeasurement will return true when fresh data is available
  if (scdSensor.readMeasurement()) {
    hq_co2_ppm = scdSensor.getCO2();
    temp_c = scdSensor.getTemperature();
    humidity_rh_perc = scdSensor.getHumidity();

    Serial.print(F("CO2(ppm):"));
    Serial.print(hq_co2_ppm);

    Serial.print(F("\tTemperature(C):"));
    Serial.print(temp_c, 1);

    Serial.print(F("\tHumidity(%RH):"));
    Serial.print(humidity_rh_perc, 1);
    Serial.println();
  }
}

// Read data points from ENS air quality sensor
void ensIter() {
  // TODO, use these functions to improve AQ sensor accuracy
  //myENS.setTempCompensationCelsius(tempC);
  //myENS.setRHCompensationFloat(rh);
  if (ensSensor.checkDataStatus()) {
    voc_ppb = ensSensor.getTVOC();
    aqi = ensSensor.getAQI();
    Serial.print("Air Quality Index (1-5) : ");
    Serial.println(aqi);

    Serial.print("Total Volatile Organic Compounds: ");
    Serial.print(voc_ppb);
    Serial.println("ppb");

    Serial.print("CO2 concentration: ");
    Serial.print(ensSensor.getECO2());
    Serial.println("ppm");
  }
}

// Update the screen
void tftIter() {
  // refreshing the screen causes flicker, so we just write on top of the
  // previous output with a wide enough column to ensure we overwrite everything.
  int val_col = 100;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // Show CO2
  tft.drawString("CO2:      ", 5, 15, 4);
  tft.drawString((String)hq_co2_ppm + " ppm               ", val_col, 15, 4);
  // Show VOC in ppb
  tft.drawString("VOC:     ", 5, 45, 4);
  tft.drawString((String)voc_ppb+" ppb               ", val_col, 45, 4);
  // Show Air Quality Index
  tft.drawString("AQI:      ", 5, 75, 4);
  tft.drawString((String)aqi+"               ", val_col, 75, 4);
  // Show Temperature in F & C
  tft.drawString("Temp:     ", 5, 105, 4);
  tft.drawString((String)(temp_c*(9.0/5.0)+32.0)+" F / "+(String)temp_c+" C               ", val_col, 105, 4);
  // Show Humidity
  tft.drawString("RHum:  ", 5, 135, 4);
  tft.drawString((String)humidity_rh_perc+" %               ", val_col, 135, 4);
}

// Main code
void loop()
{
  scdIter();
  ensIter();
  tftIter();
  delay(5000);
}
