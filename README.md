# LilyGo Air Quality Project

Monitor air quality, CO2, temperature, and humidity, displayed on a
1.9" display.

This is a very simple project to make a ~$100 standalone air quality
monitor.

The components all connect together with Qwiic cables, with USB-C
power, so there is no soldering needed, just plug-and-play.  The
Arduino IDE is used to build and upload code.

## Hardware

(approx prices in 2024)

- Lilygo T-Display-S3 ESP32 w/ grey shell ($18)
- SCD41 CO2 sensor ($65)
- ENS160 Air Quality sensor ($20)
- 2x Qwiic cables ($2)
- USB-C (for power)

## Software Libraries

### LilyGo T-Display-S3

The [TFT_eSPI](https://github.com/Xinyuan-LilyGO/T-Display-S3/) library is used to drive graphics.  I imported the code from commit [`13e743f5`](https://github.com/Xinyuan-LilyGO/T-Display-S3/tree/13e743f5653217dfd0c13d9007aff0230e9acc3c).

### SparkFun SCD41

- [SparkFun CO₂ Humidity and Temperature Sensor - SCD41 (Qwiic)](https://www.sparkfun.com/products/22396)
- [SCD4x Arduino Library](https://github.com/sparkfun/SparkFun_SCD4x_Arduino_Library)

Version 1.1.0 of the SCD4x software from Sparkfun crashed.  1.0.4 was used.

### SparkFun ENS160

- [SparkFun Indoor Air Quality Sensor - ENS160 (Qwiic)](https://www.sparkfun.com/products/20844)
- [ENS160 Arduino Library](https://github.com/sparkfun/SparkFun_Indoor_Air_Quality_Sensor-ENS160_Arduino_Library)

Version 1.0.4 was used.

## Deploying Code

The "LilyGo T-Display-S3" target is used in the Arduino IDE.
