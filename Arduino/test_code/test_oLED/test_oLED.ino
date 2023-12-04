#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int textSize = 2; // Set text size

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(textSize); // Set text size using variable
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);

  String message = "Ah-Choo!";
  int16_t x_start = (SCREEN_WIDTH - (6 * textSize * message.length())) / 2;
  int16_t y_start = (SCREEN_HEIGHT - (8 * textSize)) / 2;

  display.setCursor(x_start, y_start);
  display.println(message);
  display.display();
}

void loop() {
}
