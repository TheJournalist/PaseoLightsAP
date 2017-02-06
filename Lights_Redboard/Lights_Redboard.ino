#include <FastLED.h>

#define NUM_LEDS 60
#define CLK 2
#define PLATE 3
#define D0 4
#define D1 5
#define D2 6
#define D3 7
#define D4 8
#define D5 9
#define D6 10
#define D7 11
#define LEDSTRIP 12
#define LED 13

#define FORWARD 0
#define BACKWARD 1
#define SLOW 250
#define MEDIUM 50
#define FAST 5

void allColor(CRGB c);
void allRandom();
void turnAllOff();
void solidRGB();
void colorWipe();
void rainbow(int cycles, int speed);
void lightning(CRGB c, int simultaneous, int cycles, int speed);
void theaterChase(CRGB c, int cycles, int speed);
void cylon(CRGB c, int width, int speed);
void middleOut();
CRGB Wheel(byte WheelPos);
CRGB randomColor();

boolean direction = FORWARD;
CRGB leds[NUM_LEDS];
byte cmd = 0;
byte newcmd = 0;
byte oldcmd = 0;
byte data = 0;
byte param[3] = {0,0,0};
int nparam = 0;
bool platepr = false;

long previousMillis = 0;  
unsigned long currentMillis;

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
    pinMode(PLATE, INPUT);
    
    attachInterrupt(digitalPinToInterrupt(CLK), newdata, RISING);
    attachInterrupt(digitalPinToInterrupt(PLATE), platePressed, CHANGE);

    FastLED.addLeds<NEOPIXEL, LEDSTRIP>(leds, NUM_LEDS);

    randomSeed(analogRead(0));
}

void loop() 
{
  switch(cmd)
  {
    // Turn all off
    case 0:
      turnAllOff();
      break;
      
    // Solid RGB color
    case 1:
       solidRGB();
      break;

    // Rainbow
    case 2:
      rainbow(FAST,1);
      break;

    // Random Color Lightning
    case 3:
      lightning(NULL, 20,50,MEDIUM);
      break;

    // Theater Chase
    case 4:
      theaterChase(randomColor(),10,SLOW);
      break;

    // Cylon
    case 5:
      cylon(randomColor(), 10,FAST);
      break;

    case 6:
      middleOut();
    break;
    
    default:
      turnAllOff();
      break;
  }
}

void platePressed()
{
  platepr = !platepr;
  if(platepr)
  {
    oldcmd = cmd;
     ///cmd = 6;
    cmd = 2;
  }
  else
  {
    cmd = oldcmd;
  }
 
}

void newdata()
{
    data = 0;
    data  = digitalRead(D0);
    data |= digitalRead(D1) << 1;
    data |= digitalRead(D2) << 2;
    data |= digitalRead(D3) << 3;
    data |= digitalRead(D4) << 4;
    data |= digitalRead(D5) << 5;
    data |= digitalRead(D6) << 6;
    data |= digitalRead(D7) << 7;

    if(nparam > 0)
    {
      Serial.print("Novo parametro ");
      Serial.print(nparam);
      Serial.print(": ");
      Serial.println(data, HEX);
      param[3 - nparam] = data;
      nparam--;
      if(nparam == 0)
        cmd = newcmd;
    }
    else
    {
      newcmd = data;
      nparam = 3;
      Serial.print("Novo comando: ");
      Serial.println(data, HEX);
    } 
}

// Changes all LEDS to given color
void allColor(CRGB c){
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = c;
  }
  FastLED.show();
}

void allRandom(){
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = randomColor();
  }
  FastLED.show(); 
}

// Turn-off
void turnAllOff()
{
  for(int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Black;
  FastLED.show();
}

// Solid RGB
void solidRGB()
{
  for(int i=0; i<NUM_LEDS; i++)
  {
    leds[i].red   = param[0];
    leds[i].green = param[1];
    leds[i].blue  = param[2];
  }
  FastLED.show();
}

// Wipes color from end to end
void colorWipe()
{
  for(int i=0; i<NUM_LEDS; i++)
  {
    leds[i].red   = param[0];
    leds[i].green = param[1];
    leds[i].blue  = param[2];
   }
   FastLED.show();
   delay(param[3]+1);
}


// Rainbow colors that slowly cycle across LEDs
void rainbow(int cycles, int speed)
{ 
  int thisCmd = 2;
  if(cycles == 0)
  {
    for(int i=0; i< NUM_LEDS; i++) 
    {
      leds[i] = Wheel(((i * 256 / NUM_LEDS)) & 255);
    }
    FastLED.show();
  }
  else
  {
    for(int j=0; j<256*cycles; j++) 
    {
      for(int i=0; i< NUM_LEDS; i++) 
      {
        leds[i] = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      }
      FastLED.show();

      previousMillis = millis(); 
      currentMillis = millis(); 
      while(1)
      {
        currentMillis = millis();
        if(currentMillis - previousMillis > speed)
          break;
        if(thisCmd != cmd)
          return;
      }
      
    }
  }
}

// Random flashes of lightning
void lightning(CRGB c, int simultaneous, int cycles, int speed)
{
  int thisCmd = 3;
  int flashes[simultaneous];

  for(int i=0; i<cycles; i++)
  {
    for(int j=0; j<simultaneous; j++)
    {
      int idx = random(NUM_LEDS);
      flashes[j] = idx;
      leds[idx] = c ? c : randomColor();
    }
    FastLED.show();

    previousMillis = millis(); 
      currentMillis = millis(); 
      while(1)
      {
        currentMillis = millis();
        if(currentMillis - previousMillis > speed)
          break;
        if(thisCmd != cmd)
          return;
      }
    
    for(int s=0; s<simultaneous; s++)
    {
      leds[flashes[s]] = CRGB::Black;
    }
    
    previousMillis = millis(); 
      currentMillis = millis(); 
      while(1)
      {
        currentMillis = millis();
        if(currentMillis - previousMillis > speed)
          break;
        if(thisCmd != cmd)
          return;
      }
  }
}

// Theater-style crawling lights
void theaterChase(CRGB c, int cycles, int speed){ // TODO direction
   int thisCmd = 4;
  for (int j=0; j<cycles; j++) {  
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        int pos = i+q;
        leds[pos] = c;    //turn every third pixel on
      }
      FastLED.show();

      previousMillis = millis(); 
      currentMillis = millis(); 
      while(1)
      {
        currentMillis = millis();
        if(currentMillis - previousMillis > speed)
          break;
        if(thisCmd != cmd)
          return;
      }

      for (int i=0; i < NUM_LEDS; i=i+3) {
        leds[i+q] = CRGB::Black;        //turn every third pixel off
      }
    }
  }
}

// Sliding bar across LEDs
void cylon(CRGB c, int width, int speed){
   int thisCmd = 5;
  // First slide the leds in one direction
  for(int i = 0; i <= NUM_LEDS-width; i++) {
    for(int j=0; j<width; j++){
      leds[i+j] = c;
    }

    FastLED.show();

    // now that we've shown the leds, reset to black for next loop
    for(int j=0; j<5; j++){
      leds[i+j] = CRGB::Black;
    }
    
    previousMillis = millis(); 
    currentMillis = millis(); 
    while(1)
    {
      currentMillis = millis();
      if(currentMillis - previousMillis > speed)
        break;
      if(thisCmd != cmd)
        return;
    }
  }

  // Now go in the other direction.  
  for(int i = NUM_LEDS-width; i >= 0; i--) {
    for(int j=0; j<width; j++){
      leds[i+j] = c;
    }
    FastLED.show();
    for(int j=0; j<width; j++){
      leds[i+j] = CRGB::Black;
    }

    previousMillis = millis(); 
      currentMillis = millis(); 
      while(1)
      {
        currentMillis = millis();
        if(currentMillis - previousMillis > speed)
          break;
        if(thisCmd != cmd)
          return;
      }
  }
}

void middleOut()
{

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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
