#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

int DHT_PIN = 2;
bool swtch;
bool scale;
bool err;
int actStart;
bool serialAtch;
int serialIN;

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 2 seconds

  err = false;
  swtch = true;
  scale = true;

  display.clearDisplay();

  // Draw center line
  //display.drawLine(0, display.height()/2, display.width(), display.height()/2, SSD1306_WHITE);
  //display.display();

  // Write Titles
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("TEMP");
  display.setCursor(0,45);
  display.println("HUMD");
  display.display();

  //Write units
  display.setTextSize(1);
  display.setCursor(0,16);
  display.write(9);
  display.setCursor(5,18);
  display.println("F");
  display.display();

  int midlnT = display.height()/2+1;
  int midln = display.height()/2;
  int midlnB = display.height()/2-1;

  display.drawLine(0, midlnT, display.width()/3, midlnT, SSD1306_WHITE);
  display.drawLine(0, midln, display.width()/3, midln, SSD1306_WHITE);
  display.drawLine(0, midlnB, display.width()/3, midlnB, SSD1306_WHITE);

  display.display();
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
