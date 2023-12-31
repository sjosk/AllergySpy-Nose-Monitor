#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// DHT Sensor setup
uint8_t DHTPin = 2;        // on Pin 12 of the Huzzah
DHT dht(DHTPin, DHTTYPE);   // Initialize DHT sensor.              

void setup() {
  // open serial connection
  Serial.begin(9600);
  delay(100);

  // start DHT sensor
  pinMode(DHTPin, INPUT);
  dht.begin();
}

void loop() {
  Serial.print("Temp:");
  Serial.println(dht.readTemperature()); // Gets the values of the temperature
  Serial.print("Hum:");
  Serial.println(dht.readHumidity()); // Gets the values of the humidity
  delay(1000);
}
