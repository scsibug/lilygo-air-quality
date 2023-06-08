

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
// sensors
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
#include "SparkFun_ENS160.h"
// screen
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

// ESP32
/*ESP32S3 pin setup */
#define PIN_LCD_BL                   38

#define PIN_LCD_D0                   39
#define PIN_LCD_D1                   40
#define PIN_LCD_D2                   41
#define PIN_LCD_D3                   42
#define PIN_LCD_D4                   45
#define PIN_LCD_D5                   46
#define PIN_LCD_D6                   47
#define PIN_LCD_D7                   48

#define PIN_POWER_ON                 15

#define PIN_LCD_RES                  5
#define PIN_LCD_CS                   6
#define PIN_LCD_DC                   7
#define PIN_LCD_WR                   8
#define PIN_LCD_RD                   9

#define PIN_BUTTON_1                 0
#define PIN_BUTTON_2                 14
#define PIN_BAT_VOLT                 4

#define PIN_IIC_SCL                  17
#define PIN_IIC_SDA                  18

// Test later without these
//#define PIN_TOUCH_INT                16
//#define PIN_TOUCH_RES                21

#define I2C_SDA 43
#define I2C_SCL 44

// screen setup
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

unsigned long targetTime = 0;
byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11;
uint32_t runing = 0;


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


  // Screen setup. This was in the example but seems unnecessary?
  //pinMode(PIN_POWER_ON, OUTPUT);
  //digitalWrite(PIN_POWER_ON, HIGH);
  Serial.println("About to init TFT");
  // TFT init causes these errors, but they don't appear fatal
 //E (3623) gpio: gpio_set_level(226): GPIO output gpio_num error
 //E (3624) gpio: gpio_set_level(226): GPIO output gpio_num error
    tft.init();
    Serial.println("About to set rotation");
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.fillScreen(TFT_RED); delay(100);
    tft.fillScreen(TFT_GREEN); delay(100);
    tft.fillScreen(TFT_BLUE); delay(100);
    tft.fillScreen(TFT_BLACK); delay(100);
 // The standard ADAFruit font still works as before
    tft.setTextColor(TFT_WHITE);
    tft.setCursor (12, 5);
    tft.print("Original ADAfruit font!");
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // Do not plot the background colour
  Serial.println("Exiting Setup");
}

void scdLoop() {
  if (scdSensor.readMeasurement()) // readMeasurement will return true when fresh data is available
  {
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

void ensLoop()
{
  // TODO:
  // Give values to Air Quality Sensor to improve accuracy
	//myENS.setTempCompensationCelsius(tempC); 
	//myENS.setRHCompensationFloat(rh); 
  voc_ppb = ensSensor.getTVOC();
  aqi = ensSensor.getAQI();
	if( ensSensor.checkDataStatus() )
	{
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

void tftLoop() {
  tft.fillScreen(TFT_BLACK);
  // Show real-time high-quality CO2.
  int val_col = 90;
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Do not plot the background colour
  tft.drawString("CO2:        ", 5, 0, 2);
  tft.drawString((String)hq_co2_ppm + " ppm", val_col, 0, 2);
  // Show VOC in ppb
  tft.drawString("VOC:       ", 5, 20, 2);
  tft.drawString((String)voc_ppb+" ppb", val_col, 20, 2);
  // Show Air Quality Index
  tft.drawString("AQI:        ", 5, 40, 2);
  tft.drawString((String)aqi, val_col, 40, 2);
  // Show Temperature in F
  tft.drawString("Temp:       ", 5, 60, 2);
  tft.drawString((String)(temp_c*(9.0/5.0)+32.0)+" F", val_col, 60, 2);
  // Show Humidity
  tft.drawString("Humidity:    ", 5, 80, 2);
  tft.drawString((String)humidity_rh_perc+" %", val_col, 80, 2);
}

void loop()
{
 scdLoop();
 ensLoop();
 tftLoop();
 delay(5000);
}