
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP3XX bmp;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_HEIGHT 32                                                          // display height, pixels
#define OLED_WIDTH 128                                                          // display width, pixels
#define OLED_RESET -1                                                           // -1 .. sharing device reset pin
#define OLED_ADDRESS 0x3C                                                       // set the address of the display
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);           // set display driver

const int buttonPin = 10;                                                       // the number of the pushbutton pin
int buttonState = 0;
float groundPressureHpa = 0;
float groundLevelOffset = 0;
float curAltitude = 0;
float maxAltitude = 0;

void setup() {
  // setup serial
  Serial.begin(115200);
  while (!Serial);

  // setup button
  pinMode(buttonPin, INPUT_PULLUP);

  // setup BMP sensor
  Serial.println("Setup BMP3xx sensor");
  if (!bmp.begin_I2C()) {                                                     // begin BMP sensor
    Serial.println("BMP3xx sensor not found");
    while (1);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_32X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // setup OLED display
  Serial.println("Setup display");
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {                  // begin display
    Serial.println(F("OLED display not found"));  
    while (1);
  }
  display.display();                                                        // Show initial display buffer contents
  delay(1000);                                                              // Pause for seconds
  display.clearDisplay();   // Clear the buffer
}

void loop() {
  if (! bmp.performReading()) {                                             // check that BMP reads
    Serial.println("Failed to perform BMP reading :(");
    return;                                                                 // exit if no BMP reading
  }

  buttonState = digitalRead(buttonPin);                                     // get button state
  if (millis() < 3000 || buttonState == LOW) {                              // if button pushed or just starting
    maxAltitude = 0;                                                        // set to 0m hight
    groundLevelOffset = bmp.readAltitude(SEALEVELPRESSURE_HPA);             // set ground level offset
  }
  curAltitude = bmp.readAltitude(SEALEVELPRESSURE_HPA) - groundLevelOffset; // set current altitude
  if (curAltitude > maxAltitude && curAltitude > 1) {                       // if higher than recorded Max height and > 1m
     maxAltitude = curAltitude;                                             // set max height to current height
  }

  display.clearDisplay();                                                   // clear display
  display.setTextColor(SSD1306_WHITE);                                      // reset diplay settings
  display.setCursor(5,10);                                                  // set cursor start
  display.setTextSize(2);                                                   // set font size
  display.print(maxAltitude);                                               // write altitude to buffer
  display.println(" m");                                                    // write "m" and end line to buffer
  display.display();                                                        // send buffer to display
}
