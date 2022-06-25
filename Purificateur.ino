#include <FastLED.h>
#define NUM_LEDS 71
#define DATA_PIN 13
CRGB leds[NUM_LEDS];

#define        COV_RATIO                       0.2            //ug/mmm / mv
#define        NO_DUST_VOLTAGE                 400            //mv
#define        SYS_VOLTAGE                     5000

const int iled = 7;                                            //drive the led of sensor
const int vout = A0;                                            //analog input
float density, voltage;
int   adcvalue;
int Filter(int m)
{
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;

  if (flag_first == 0)
  {
    flag_first = 1;
    for (i = 0, sum = 0; i < _buff_max; i++)
    {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }
  else
  {
    sum -= _buff[0];
    for (i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];

    i = sum / 10.0;
    return i;
  }
}


void setup(void)
{
  pinMode(iled, OUTPUT);
  digitalWrite(iled, LOW);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  Serial.begin(9600);                                         //send and receive at 9600 baud
  Serial.print("*********************************** Dust Purifier by LFPoulain ***********************************\n");
  FastLED.setBrightness(255);

}

void loop(void)
{
  digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);
  adcvalue = Filter(adcvalue);
  voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;

  if (voltage >= NO_DUST_VOLTAGE)
  {
    voltage -= NO_DUST_VOLTAGE;

    density = voltage * COV_RATIO;
  }
  else

    density = 0;

  Serial.print("The current dust concentration is: ");
  Serial.print(density);
  Serial.print(" ug/m3\n");
  int mappeddensityred;
  mappeddensityred = map(density, 35, 150, 0, 255);
  int mappeddensitygreen;
  mappeddensitygreen = map(density, 35, 150, 255, 0);

  if (density < 35 )   fill_solid(leds, NUM_LEDS, CRGB::Green);
  if (density > 150)   fill_solid(leds, NUM_LEDS, CRGB::Red);
  if (density > 35 && density < 150) {
    fill_solid( leds, NUM_LEDS, CRGB( mappeddensityred, mappeddensitygreen, 0));
  }
  FastLED.show();
  delay(1000);
}
