/* Waveshare Dust Sensor tested with Arduino UNO, Michael H March 30, 2021
  VCC red => 3.3V
  GND black => GND
  AOUT blue => A0
  ILED yellow => A2
*/

long int systemTime;    // replaces delay()
int AoutMin = 360;      // equal to Aout min in mV for clean air
int AoutMax = 3600;     // equal to Aout max in mV (toothpick in measuring window)
float dustConst;        // it will be computed from Aout range and Sharp sensor range 0-500 ug/m3
int dustConc;           // dust concentration in ug/m3/mV
unsigned int sum11;     // sum of 11 counts to compensate 11:1 Aot voltage divider
int AoutAvg;            // direct Sharp Vout in mV computed as average from 11 measurements
int ref1024 = 1100;     // reference voltage for 10-bit ADC; 1023 counts = 1100 mV
int dustQualityIndex;   // it is quite arbitrary
char *dustQualityChar[] = {"Excellent", "Very good", "Good", "Normal", "Poor"};

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);  // pin selected for 3.3V, GND, AOUT, and ILED
  pinMode(A2, OUTPUT); // in order RED, BLACK, BLUE, and YELLOW as on WaveShare board
  analogReference(INTERNAL); // for better resolution use 1100mV instead of default 5000mV
  analogRead(A0);       // activate IOref pin, 1.1V will be present
  dustConst = 500 / float(AoutMax - AoutMin); // in ug/m3 per millivolt
  Serial.print("\n ------------------------------------------------------\n");
  Serial.print(" The WaveShare board divides the Sharp Sensor Vout 11:1.\n");
  Serial.print(" The sum of 11 counts will compensate for lowered output.\n");
  Serial.print(" Then the sum is converted to mV, and from it\n");
  Serial.print(" is computed dust concentration and air quality.");
  Serial.print("\n-------------------------------------------------------\n");
  Serial.print("500/(");
  Serial.print(AoutMax);
  Serial.print("-");
  Serial.print(AoutMin);
  Serial.print(")=");
  Serial.print(dustConst);
  Serial.print(" Dust density constant in ug/m3/mV");
  Serial.print("\n-------------------------------------------------------\n");
}
void loop() {
  sum11 = 0;
  for (int i = 0; i < 11; i++) {
    //Sharp datasheet: pulse cycle 10ms, pulse width 0.32ms, sampling at 0,28ms
    digitalWrite(A2, HIGH);
    delayMicroseconds(280);
    sum11 += analogRead(A0);
    delayMicroseconds(40);   // 280+40=320
    digitalWrite(A2, LOW);
    delayMicroseconds(9680); // 320 + 9680 = 10000
  }
  if ((millis() - systemTime) > 2000) {
    computeAirQuality();
    systemTime = millis();
  }
}

void computeAirQuality()
{
  AoutAvg = float(sum11) * float(ref1024) / 1024; // Sharp sensor direct Aout in mV
  int q, i;
  q = float(AoutAvg - AoutMin) * dustConst;
  if (q < 0) q = 0; // handle non positive values

  if (q < 40) i = 0;
  else if (q < 80) i = 1;
  else if (q < 160) i = 2;
  else if (q < 320) i = 3;
  else i = 4;

  dustConc = q; dustQualityIndex = i;
  printAirQuality();
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
