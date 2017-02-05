#include <Adafruit_NeoPixel.h>

#define CLK 2
#define D0 3
#define D1 4
#define D2 5
#define D3 6
#define D4 7
#define D5 8
#define D6 9
#define D7 10
#define LEDSTRIP 11
#define LED 13

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, LEDSTRIP, NEO_GRB + NEO_KHZ800);

byte cmd = 0;

void setup() 
{
    Serial.begin(9600);
    Serial.println("Iniciado...");
    pinMode(D0, INPUT);
    pinMode(D1, INPUT);
    pinMode(D2, INPUT);
    pinMode(D3, INPUT);
    pinMode(D4, INPUT);
    pinMode(D5, INPUT);
    pinMode(D6, INPUT);
    pinMode(D7, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(CLK, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLK), newdata, RISING);
    
    strip.begin();
    strip.show();
}

void loop() 
{
  switch(cmd)
  {
    case 0:
      strip.show();
      break;
    case 1:
      colorWipe(strip.Color(255, 0, 0), 50); // Red
      break;
    case 2:
      colorWipe(strip.Color(0, 255, 0), 50); // Green
      break;
    case 3:
      colorWipe(strip.Color(0, 0, 255), 50); // Blue
      break;
    case 4:
      rainbow(20);
      break;
    default:
      strip.show();
      break;
  }

  /*
  if(on)
  {
    // Some example procedures showing how to display to the pixels:
    colorWipe(strip.Color(255, 0, 0), 50); // Red
    colorWipe(strip.Color(0, 255, 0), 50); // Green
    colorWipe(strip.Color(0, 0, 255), 50); // Blue
    // Send a theater pixel chase in...
    theaterChase(strip.Color(127, 127, 127), 50); // White
    theaterChase(strip.Color(127,   0,   0), 50); // Red
    theaterChase(strip.Color(  0,   0, 127), 50); // Blue

    rainbow(20);
    rainbowCycle(20);
    theaterChaseRainbow(50);
  }
  */

}

void newdata()
{
    cmd = 0;
    cmd  = digitalRead(D0);
    cmd |= digitalRead(D1) << 1;
    cmd |= digitalRead(D2) << 2;
    cmd |= digitalRead(D3) << 3;
    cmd |= digitalRead(D4) << 4;
    cmd |= digitalRead(D5) << 5;
    cmd |= digitalRead(D6) << 6;
    cmd |= digitalRead(D7) << 7;
    Serial.print("Novo comando: ");
    Serial.println(cmd, HEX);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i<strip.numPixels(); i++) 
    {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

