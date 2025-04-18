# ESP-IDF New I2C ADXL345 Demo

<H2>Quick demo showing how to use the "new" ESP-IDF I2C master library utilizing and ADXL345 I2C accelerometer.</H2>

I wanted to do a quick "scratchpad" project to figure out how the new [ESP-IDF I2C](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html) library worked, and I also wanted to play around with an I2C accelerometer for an upcoming project.  This worked well enough that I thought it might be a good reference for working with the ESP-IDF I2C library in the future.

&nbsp; 

<p align="center">
    <img src="./resources/adxl345_demo.gif" alt="ADXL345 Demo"/>
</p>

The ADXL345 accelerometer here is controlled via the ESP-IDF [I2C master utility](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html).  The character LCD display is controlled via my own [Hitachi HD44780 Driver](https://github.com/TheFlemoid/ESP-IDF-HD44780).  

This repo is mostly stood up to be a reference of the steps necessary to set up any random I2C device via ESP-IDF standard libraries, however if you'd like to actually build and run this project the ESP-32 pinout is as follows:

| ESP-32 | I2C Pin |
| :---: | :---: |
| GPIO 21 | SDA |
| GPIO 22 | SCL | 

| ESP-32 | HD44780 Pin |
| :---: | :---: |
| GPIO 25 | D4 |
| GPIO 26 | D5 |
| GPIO 27 | D6 |
| GPIO 32 | D7 |
| GPIO 17 | RS |
| GPIO 19 | E |
| GND | RW |

That said, this repo is mostly designed to be a reference on the utilization of the ESP-IDF I2C master library.  A review of the repos [sole source file](./main/main.c) should show step by step instructions on how to initialize an I2C bus, register an I2C device, and actually communicate with said device.
