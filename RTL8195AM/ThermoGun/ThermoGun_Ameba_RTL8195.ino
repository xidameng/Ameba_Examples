/**************************************************************************
 Title: Ameba ThermoGun
 
 Items:
  * Ameba RTL8195AM               x1
  * Push button (on pin 9)        x1
  * MLX90615 temp sensor (I2c)    x1
  * SSD1306 OLED display          x1

 The widespread of COVID-19 is causing demand for non-contact thermometer to skyrocket 
 in the past few months, this project use Realtek Ameba RTL8195AM and Melexis MLX90615
 temperature sensor to create a homemade thermometer hoping to help fighting the virus
 
 This example uses a 128x64 pixel display（Monochrome OLEDs based on SSD1306 drivers） 
 using SPI to communicate, 4 or 5 pins are required to interface.

 Adafruit OLED library is used, thus credit to adafruit too. (library can be downloaded
 using Arduino IDE's library manager)

 Created by: xidameng
 Date:       30/04/202

 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// MLX90615 Temperature sensor setup
#define MLX90615_I2C_ADDR    0x5B         // factory default address 
#define MLX90615_REG_TEMP_AMBIENT  0x26
#define MLX90615_REG_TEMP_OBJECT  0x27    // 0x27 = 0b00100111, is a command, meaning reading from sensor RAM and read object temperature
float tempdata = 0;

//SSD1306 OLED display setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Only 4 pins are needed 
#define OLED_RESET 10  //(RES)
#define OLED_MOSI  11  //(SDA)
#define OLED_DC    12  //(DC)
#define OLED_CLK   13  //(SCL)
#define OLED_CS    17  //(Not Connected)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


#define LOGO_HEIGHT   64
#define LOGO_WIDTH    64

char title[] = " Ameba ThermoGun ";
char spacer[] = "                 ";

static const unsigned char PROGMEM logo_bmp[] =
{
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B01110000,B01110000,B00000000,B00000000,B00000111,B00000111,B00000000,
B00000000,B01110000,B01110000,B00000000,B00000000,B00000111,B00000111,B00000000,
B00000000,B01110000,B01110000,B00000000,B00000000,B00000111,B00000111,B00000000,
B00000000,B01111001,B11110000,B00000000,B00000000,B00000111,B11001111,B00000000,
B00000000,B01111001,B11110000,B00000000,B00000000,B00000111,B11001111,B00000000,
B00000000,B01111101,B11110000,B00000000,B00000000,B00000111,B11011111,B00000000,
B00000000,B01111111,B11110000,B00000000,B00000000,B00000111,B11111111,B00000000,
B00000000,B01111111,B11110000,B00000000,B00000000,B00000111,B11111111,B00000000,
B00000000,B01111111,B11110000,B00000000,B00000000,B00000111,B11111111,B00000000,
B00000000,B00001111,B10000000,B00000000,B00000000,B00000000,B11111000,B00000000,
B00000000,B00001111,B10000000,B00000000,B00000000,B00000000,B11111000,B00000000,
B00000000,B00001111,B10000000,B00000000,B00000000,B00000000,B11111000,B00000000,
B00000000,B00001111,B10000000,B00000000,B00000000,B00000000,B11111000,B00000000,
B00000000,B00001111,B10000000,B00111100,B00011110,B00000000,B11111000,B00000000,
B00000000,B00001111,B11000000,B00111100,B00011110,B00000001,B11111000,B00000000,
B00000000,B00001111,B11100000,B00111100,B00011110,B00000011,B11111000,B00000000,
B00000000,B00001111,B11110000,B00111100,B00011110,B00000111,B11111000,B00000000,
B00000000,B00000111,B11111000,B00011000,B00001100,B00001111,B11110000,B00000000,
B00000000,B00000011,B11111100,B00001000,B00001100,B00011111,B11100000,B00000000,
B00000000,B00000001,B11111110,B00001000,B00001100,B00111111,B11000000,B00000000,
B00000000,B00000000,B11111110,B00001000,B00001100,B00111111,B10000000,B00000000,
B00000000,B00000000,B01111110,B00001000,B00001100,B00111111,B00000000,B00000000,
B00000000,B00000000,B01111110,B00001111,B11111100,B00111111,B00000000,B00000000,
B00000000,B00000000,B01111110,B00000000,B00000000,B00111111,B00000000,B00000000,
B00000000,B00000000,B01111110,B00000000,B00000000,B00111111,B00000000,B00000000,
B00000000,B00000000,B01111110,B00000000,B00000000,B00111111,B00000000,B00000000,
B00000000,B00000000,B01111111,B11110000,B00000111,B11111100,B00000000,B00000000,
B00000000,B00000000,B00111111,B11111000,B00001111,B11111100,B00000000,B00000000,
B00000000,B11111111,B10111111,B11111100,B00011111,B11111101,B11111111,B00000000,
B00000001,B11000000,B00111111,B11111110,B00111111,B11111100,B00000011,B00000000,
B00000001,B10000000,B00111111,B11111111,B00111111,B11111000,B00000001,B10000000,
B00000011,B00011111,B10110000,B11111111,B11111110,B00000001,B11111000,B11000000,
B00000110,B00111111,B00111111,B11111111,B11111110,B00000001,B11111100,B01100000,
B00000110,B01110000,B00111111,B11111111,B11111110,B00000000,B00001110,B01100000,
B00000100,B11100000,B00111111,B11111011,B11101110,B00000000,B00000110,B00100000,
B00000100,B11000111,B10111011,B11111001,B11001110,B00000001,B11110011,B00100000,
B00000100,B10001111,B00110001,B11111000,B10001110,B00000000,B11110011,B00100000,
B00000100,B10011000,B00110001,B11111000,B00001110,B00000000,B00011001,B00100000,
B00000100,B10010001,B10110001,B11111000,B00001110,B00000001,B10001101,B00100000,
B00000100,B10010011,B10110000,B11111000,B00001111,B00000001,B11001101,B00100000,
B00000000,B10010010,B00110000,B11111000,B00001111,B11111000,B01001101,B00000000,
B00000000,B10010010,B00110000,B11111000,B00001111,B11111000,B01001101,B00000000,
B00000000,B00010010,B00000000,B00000000,B00000000,B00000000,B01001100,B00000000,
B00000000,B00010010,B00000000,B00000000,B00000000,B00000000,B01001100,B00000000,
B00000000,B00000010,B00000000,B00000000,B00000000,B00000000,B01000000,B00000000,
B00000000,B00000010,B00000000,B00000000,B00000000,B00000000,B01000000,B00000000,
B00000000,B00000010,B00000000,B00000000,B00000000,B00000000,B01000000,B00000000,
B00111111,B10000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00111001,B11000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00111000,B11000111,B11100000,B10000001,B10000111,B11110011,B11100011,B00001000,
B00011000,B11100110,B00000000,B11000001,B10000001,B11000011,B00000011,B00010000,
B00011000,B11000110,B00000001,B11000001,B10000000,B11000011,B00000011,B00100000,
B00011001,B11000110,B00000001,B11000001,B10000000,B11000011,B00000011,B01000000,
B00011111,B00000110,B00000000,B01100001,B10000000,B11000011,B00000011,B01000000,
B00011011,B00000111,B11000010,B01100001,B10000000,B11000011,B11100011,B11000000,
B00011011,B10000110,B00000010,B01110001,B10000000,B11000011,B00000011,B11100000,
B00011001,B11000110,B00000110,B00110001,B10000000,B11000011,B00000011,B01100000,
B00111001,B11000110,B00000100,B00110001,B10000000,B11000011,B00000011,B00110000,
B00111000,B11100110,B00001000,B00011001,B10000000,B11000011,B00000011,B00111000,
B00011000,B01100111,B11101000,B00011001,B11111000,B10000011,B11100011,B00011000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000 

};


float get_temp(){
  
  uint16_t data;

  Wire.beginTransmission(MLX90615_I2C_ADDR);
  Wire.write(MLX90615_REG_TEMP_OBJECT);
  Wire.endTransmission(false); // false is to instruct i2c NOT to send a STOP bit 
  
  Wire.requestFrom(MLX90615_I2C_ADDR, 2); // read 3 bytes of data
  data = Wire.read();       // 1st byte is LSB
  data|= Wire.read() << 8;  // 2nd byte is MSB
  //Wire.read();              // 3rd byte is PEC, which is not of our interest here. This line is just to exhaust i2c buffer
  
//  Serial.println();
//  Serial.print("Data received is: ");
//  Serial.println(data);
//  Serial.println(data,BIN);
  
  return tempdata = data * 0.02 - 273.15; //raw temperateu data is in kelvin thus requiring conversion to celsius
}


void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(9,INPUT); // push button on pin 9

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  drawbitmap();    // Draw a small bitmap image
  delay(3000);
  interface();
}

void loop() {
  
  while(digitalRead(9)) {
    tempdata = get_temp();
    Serial.print(" Object= ");
    Serial.print(tempdata);
    Serial.println(" *C");
    interface();
  }
}


void interface(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);        // Draw 'inverse' text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(spacer);
  display.println(title);
  display.println(spacer);
  display.println(); 

  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE); // Draw 'inverse' text
  display.println(F("Temperature:"));
  display.println(); 

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(tempdata); 
  display.println(F(" C"));

  display.display();
  delay(1000);
}


void drawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}
