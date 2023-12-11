#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

// Define DHT22 
#define DHTPIN 2     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

// Define OLED 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define Dust sensor
long int systemTime; // Equivalent to Aout min in mV for clean air
int AoutMin = 360; // Equivalent to Aout max in mV (toothpick in measuring window)
int AoutMax = 3600; // Equivalent to Aout max in mV (toothpick in measuring window)
float dustConst; // Will be computed from Aout range and Sharp sensor range 0-500 ug/m3
int dustConc;  // Dust concentration in ug/m3/mV
unsigned int sum11; // Sum of 11 counts to compensate 11:1 Aot voltage divider
int AoutAvg; // Direct Sharp Vout in mV computed as average from 11 measurements
int ref1024 = 1100; // Reference voltage for 10-bit ADC; 1023 counts = 1100 mV
int dustQualityIndex; // Quite arbitrary
char *dustQualityChar[] = {"Excellent", "Very good", "Good", "Normal", "Poor"};

// Define NeoPixel
#define LED_PIN  6
#define LED_COUNT 8
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // loop
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Initialize NeoPixel
  strip.begin();
  strip.show(); //Initialize all NeoPixel to 'off'

  // Initialize Dust Sensor
  pinMode(A0, INPUT);
  pinMode(A2, OUTPUT);
  analogReference(INTERNAL);
  analogRead(A0);  // Activate IOref pin, 1.1V will be provided
  dustConst = 500 / float(AoutMax - AoutMin);
}

void loop() {
  // Read dust sensor data
  sum11 = 0;
  for (int i = 0; i < 11; i++) {
    digitalWrite(A2, HIGH);
    delayMicroseconds(280);
    sum11 += analogRead(A0);
    delayMicroseconds(40);
    digitalWrite(A2, LOW);
    delayMicroseconds(9680);
  }
 if ((millis() - systemTime) > 5000) {
    computeAirQuality();
    systemTime = millis();
    
  // Read DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Update NeoPixel color
      if (dustQualityIndex == 4) { // If Air Quality = Poor
      setStripColor(strip.Color(255, 0, 0)); // Red
    } else if (humidity > 50) {
      setStripColor(strip.Color(255, 165, 0)); // Orange
    } else {
      // update NeoPixel color based on air quality and humidity
      switch (dustQualityIndex) {
        case 0: // Excellent
        case 1: // Very good
        case 2: // Good
          setStripColor(strip.Color(0, 255, 0)); // Green
          break;
        case 3: // Normal
          setStripColor(strip.Color(0, 0, 255)); // Blue
          break;
      }
    }

  // Update OLED display
    display.clearDisplay();
    display.setTextSize(2); 
    display.setTextColor(SSD1306_WHITE);
    String text = "Ah-Choo! \n";
    //int16_t textWidth = 12 * text.length(); 
    //int16_t x_start = (SCREEN_WIDTH - (11 * text.length())) / 2;
    display.setCursor(22, 0); //set it to center
    display.println(text);
    display.setTextSize(1);

    // Print Temp.
    display.setCursor(10, 20); // Set (x,y)
    display.print(" Temp: ");
    display.print(temperature);
    display.println(" *C");

    // Print Humidity
    display.setCursor(10, 30); 
    display.print(" Humidity: ");
    display.print(humidity);
    display.println(" %");

    // Print Dust
    display.setCursor(10, 40); 
    display.print(" Dust: ");
    display.print(dustConc);
    display.println(" ug/m3");

    // Print Air quality
    display.setCursor(10, 50); 
    display.print(" AQ: ");
    display.println(dustQualityChar[dustQualityIndex]);
    
    

    display.display();
  }
}

void setStripColor(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

//compute air quality
void computeAirQuality() {
  AoutAvg = float(sum11) * float(ref1024) / 1024;
  int q = float(AoutAvg - AoutMin) * dustConst;
  if (q < 0) q = 0;

  int i;
  if (q < 40) i = 0;
  else if (q < 80) i = 1;
  else if (q < 160) i = 2;
  else if (q < 320) i = 3;
  else i = 4;

  dustConc = q; dustQualityIndex = i;
  printAirQuality();
  Serial.println("Air Quality Computed.");
}
void printAirQuality()
{
  Serial.print(" Sum_11 ");
  Serial.print(sum11);
  Serial.print(" \t ");
  Serial.print(AoutAvg);
  Serial.print(" mV \tdust_C ");
  Serial.print(dustConc);
  Serial.print(" ug/m3\t\t");
  Serial.print(dustQualityChar[dustQualityIndex]);
  Serial.print(" air quality\n");
}
