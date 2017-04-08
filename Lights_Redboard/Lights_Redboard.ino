#include "FastLED.h"
#include <SoftwareSerial.h>

#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define ddrOfPin(P)\
  (((P)>=0&&(P)<8)?&DDRD:(((P)>7&&(P)<14)?&DDRB:&DDRC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P)((uint8_t)(P>13?P-14:P&7))
#define pinMask(P)((uint8_t)(1<<pinIndex(P)))

#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P)((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P)((uint8_t)isHigh(P))

#define LEDSTRIP   12 // pin connected to the NeoPixels strip
#define NUM_LEDS   180 // number of LEDs on strip

// Ledstrip
CRGB leds[NUM_LEDS];
CRGB rainbow_leds[25];
CRGB real_leds[NUM_LEDS];

// Edison Serial
SoftwareSerial mySerial(5, 6); // RX, TX

unsigned long patternInterval = 20 ; // time between steps in the pattern
unsigned long lastUpdate = 0 ; // for millis() when last update occoured
unsigned long intervals [] = { 20, 20, 50, 100 } ; // speed for each pattern

typedef struct
{
  int platePin;
  bool platePressed = false;  
} Plate;
Plate pressure_plates[8];
int plate_pins[8] = {2,3,4,7,8,9,10,11};

class LedStrip
{
  public:
  int startLed;
  int endLed;

  LedStrip(int _startLed, int _endLed)
  {
    startLed = _startLed;
    endLed = _endLed;
  }
};

LedStrip ledStrips[8] = 
    {
      LedStrip(0, 22), 
      LedStrip(22, 45), 
      LedStrip(45, 67),   
      LedStrip(67, 90),   
      LedStrip(90, 112),
      LedStrip(112, 135),
      LedStrip(135, 157),
      LedStrip(157, 180)
    };

byte cmd = 0;
uint8_t gHue = 0;

bool navi = false;
int dest = 0;
int currentPos = -2;
CRGB rc;

void setup() 
{
  // Debug
  Serial.begin(9600);
  Serial.println("Starting...");
  // Edison Serial
  mySerial.begin(9600);
  // Pressure plates
  for(int i=0; i<8; i++)
  {
    pinMode(plate_pins[i], INPUT_PULLUP);
    pressure_plates[i].platePin = plate_pins[i];
  }
  // Led strip
  FastLED.addLeds<NEOPIXEL, LEDSTRIP>(real_leds, NUM_LEDS);
}

void loop() 
{
  // Edison Serial
  if (mySerial.available())  // If message was received
  {
    // New command received
    cmd = mySerial.read();
    currentPos = -2;
    wipe();
    
    // Debug
    Serial.print("CMD: ");
    Serial.print(cmd, HEX);
    Serial.print("\n");
    /*switch(cmd) 
    {
      case 0:
          Serial.print(" - Clear LEDS\n");
          break;
      case 1: 
          Serial.print(" - Random Solid Color\n");
          break;
      case 2:
          Serial.print(" - Rainbow\n");
          break;
      case 3:
          Serial.print(" - Juggle\n");
          break;     
      case 4:
          Serial.print(" - Theater chase\n");
          break;
      case 5:
          Serial.print(" - Cylon\n");
          break;
      case 6:
          Serial.print(" - Confetti\n");
          break;
      case 7:
          Serial.print(" - BMP\n");
          break;
      case 8:
          Serial.print(" - Destination: Plate 0\n");
          break;
      case 9:
          Serial.print(" - Destination: Plate 2\n");
          break;
      case 10:
          Serial.print(" - Destination: Plate 5\n");
          break;
      case 11:
          Serial.print(" - Destination: Plate 6\n");
          break;
      default:
          Serial.print(" - ??? Clear LEDS\n");
          break;
    } */
    rc = randomColor();
  }

  updatePlateState();

  if(millis() - lastUpdate > patternInterval) 
    updatePattern(cmd);

}

void updatePattern(int cmd)
{
  if((cmd < 8) || (cmd == 12))
    navi = false;
  else
    navi = true;
  
  switch(cmd) 
  {
    case 0:
        wipe(); 
        break;
    case 1: 
        solidColor(rc);
        break;
    case 2:
        rainbowCycle();
        break;
    case 3:
        juggle();
        break;     
    case 4:
        theaterChaseRainbow(); 
        break;
    case 5:
        sinelon();
        break;
    case 6:
        confetti();
        break;
    case 7:
        bpm();
        break;
    case 8:
        dest = 0;
        break;
    case 9:
        dest = 2;
        break;
    case 10:
        dest = 5;
        break;
    case 11:
        dest = 6;
        break;
    case 12:
        game();
        break;
    default:
        navi = false;
        currentPos = -1;
        wipe();
        break;
  } 
  
  if(cmd == 12)
    cmd = 2;
  
  rainbowing();
}

void game()
{
  // 63542178
  int path[8] = {5,2,4,3,1,0,6,7};
  unsigned long startTime = millis();
  bool timeout = false;

  for(int i = 0; i<NUM_LEDS; i++)
    real_leds[i] = CRGB::Black;
  
  Serial.print("Game started!");
  for(int i = 0; i<8; i++)
  {
    for(int j = ledStrips[path[i]].startLed; j<ledStrips[path[i]].endLed; j++)
      real_leds[j] = CRGB::LightBlue;

    FastLED.show();
    while(digitalState(pressure_plates[path[i]].platePin) == 1)
    {
      if(millis() - startTime > 60000)
      {
        timeout = true;
        break;
      }
    }

    for(int j = ledStrips[path[i]].startLed; j<ledStrips[path[i]].endLed; j++)
      real_leds[j] = CRGB::Black;

    if(timeout)
      break;
  }
  unsigned long endTime = millis() - startTime;
  Serial.print("Tempo(ms): ");
  Serial.print(endTime);
  Serial.print("\n");
}

void rainbowing()
{
  static uint16_t k=0;
  for(int i=0; i<8; i++)
    for(int i=0; i< 25; i++) {
      rainbow_leds[i] = Wheel(((i * 256 / 25) + k) & 255);
    }
  k++;
  if(k >= 256*5) k=0;
  
  for(int i = 0; i < NUM_LEDS; i++)
    real_leds[i] = leds[i];
  
  for(int i=0; i<8; i++)
  {
    if(pressure_plates[i].platePressed == true)
    {
      for(int j = ledStrips[i].startLed; j<ledStrips[i].endLed; j++)
      {
        real_leds[j] = rainbow_leds[ledStrips[i].endLed - j];
      }
    }

    if(navi)
    {
      if(i == currentPos)
      {
        for(int j = ledStrips[i].startLed; j<ledStrips[i].endLed; j++)
        {
          if(currentPos != dest)
            real_leds[j] = CRGB::Red;
          else
            real_leds[j] = rainbow_leds[ledStrips[i].endLed - j];
        }
      }
      else if((currentPos > dest) && (i == currentPos-1))
      {
        for(int j = ledStrips[i].startLed; j<ledStrips[i].endLed; j++)
            real_leds[j] = CRGB::Green;
      }
      else if((currentPos < dest) && (i == currentPos+1))
      {
        for(int j = ledStrips[i].startLed; j<ledStrips[i].endLed; j++)
            real_leds[j] = CRGB::Green;
      }
      else
      {
        for(int j = ledStrips[i].startLed; j<ledStrips[i].endLed; j++)
            real_leds[j] = CRGB::Black;
      }
    }
    
  }
  FastLED.show();
}

void solidColor(CRGB c)
{
  for ( int i = 0; i < NUM_LEDS; i++)
    leds[i] = c;
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( random8(128)+127, random8(128)+127, random8(128)+127);
}

void rainbowCycle() 
{
  static uint16_t j=0;
    for(int i=0; i< NUM_LEDS; i++) {
      leds[i] = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
    }
  j++;
  if(j >= 256*8) j=0;
  lastUpdate = millis(); // time for next change to the display
}

void theaterChaseRainbow() 
{ 
  static int j=0, q = 0;
  static boolean on = true;
     if(on){
            for (int i=0; i < NUM_LEDS; i=i+3) {
                leds[i+q] = Wheel( (i+j) % 255);    //turn every third pixel on
             }
     }
      else {
           for (int i=0; i < NUM_LEDS; i=i+3) {
               leds[i+q] = 0;        //turn every third pixel off
                 }
      }
     on = !on; // toggel pixelse on or off for next time
      q++; // update the q variable
      if(q >=3 ){ // if it overflows reset it and update the J variable
        q=0;
        j++;
        if(j >= 256) j = 0;
      }
  lastUpdate = millis(); // time for next change to the display    
}

void wipe()
{
   for(int i=0;i<NUM_LEDS;i++)
       leds[i] = CRGB::Black;
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CHSV( 0, 255, 192);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void updatePlateState()
{
  for(int i=0; i<8; i++)
  {
    if(digitalState(pressure_plates[i].platePin) == 0)
    {
      if(pressure_plates[i].platePressed == false)
      {
        Serial.print("PLATE PRESSED: ");
        Serial.print(i);
        Serial.print("\n");
        mySerial.write(i);
        
        if(navi)
        {
          if(currentPos == -2)
          {
            Serial.print("Initial position: Plate ");
            Serial.print(i);
            Serial.print("\n");
            currentPos = i;
          }
          else if((currentPos > dest) && (i == currentPos-1))
          {
            Serial.print("Current position: Plate ");
            Serial.print(i);
            Serial.print("\n");
            currentPos = i;
          }
          else if((currentPos < dest) && (i == currentPos+1))
          {
            Serial.print("Current position: Plate ");
            Serial.print(i);
            Serial.print("\n");
            currentPos = i;
          }

          if(currentPos == dest)
          {
            mySerial.write(15);
          }
        } 
      }
      pressure_plates[i].platePressed = true;
    }
    else
    {
      pressure_plates[i].platePressed = false;
    }
  }
}

CRGB Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

CRGB randomColor(){
  return Wheel(random(256)); 
}
