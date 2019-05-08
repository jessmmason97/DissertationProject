// =====================================================
//
// Colour Sensing Robot for Dissertation Project
// (Jessica Mason - 16016599)
//
// =====================================================

#include <Wire.h>
#include "Adafruit_TCS34725.h"

//DEFINE STATES
#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3
#define STATE_4 4
#define STATE_5 5
#define STATE_6 6

//DEFINE COLOURS
#define COLOUR_RED 10
#define COLOUR_GREEN 11
#define COLOUR_BLUE 12
#define COLOUR_YELLOW 13
#define COLOUR_BLACK 14

//NORMALISED RGB VALUES
double rNorm;
double gNorm;
double bNorm;
//HUE
double h;
double hDegrees;
//DEBUG
double g; 
//EQUATION SECTIONS
double topA;
double bottomA;
double bottomB;
//PI values (pretty sure these are built in, check that)
double Pi = 3.1415926535897932384626433832795;
double twoPi = 6.283185307179586476925286766559;

//COLOUR SENSING VARIABLES
int currentColour;
int prevColourX;
int prevColourY;
int outputColour;

//GENERAL VARIABLES
//CURRENT STATE
int state;
//COUNT ON X AXIS
int countX;
//COUNT ON Y AXIS
int countY;
bool detected;

//ARRAYS
int pathX[10];
int indexX = 0;
int pathY[10];
int indexY = 0;

//INITIALISE COLOUR SENSOR
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();
/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup(void) {

  //set state to state0 which is when the robot goes from a line onto the background
  state = STATE_0;
  countX = 0;
  countY = 0;
  prevColourX = COLOUR_BLACK;
  prevColourY = COLOUR_BLACK;
  detected = false;
  
  Serial.begin(9600);

  //CHECK COLOUR SENSOR IS CONNECTED
  if (tcs.begin()) {
    Serial.println("Found sensor");
    Serial.println(" ");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    Serial.println(" ");
    while (1);
  }
 
}// END OF setup()

void loop(void) {
 
  //this runs the get colour function and recieves the outputted colour
  currentColour = getColour();
  
  //FININTE STATE MACHINE
  switch(state){

    case STATE_0:
      //GOES ONTO A RED LINE
      if((currentColour == COLOUR_RED) && (prevColourX == COLOUR_BLACK)){
        state = STATE_1;
        prevColourX = COLOUR_RED;
      }
      //GOES ONTO A GREEN LINE
      if((currentColour == COLOUR_GREEN) && (prevColourX == COLOUR_BLACK)){
        state = STATE_2;
        prevColourX = COLOUR_GREEN;
      }
      //GOES ONTO A YELLOW LINE
      if((currentColour == COLOUR_YELLOW) && (prevColourY == COLOUR_BLACK)){
        state = STATE_3;
        prevColourY = COLOUR_YELLOW;
      }
      //GOES ONTO A BLUE LINE
      if((currentColour == COLOUR_BLUE) && (prevColourX == COLOUR_BLACK)){
        state = STATE_4;
        prevColourY = COLOUR_BLUE;
      }


    //RED BEHAVIOUR
    case STATE_1:
      //GOING OVER A RED LINE 
      if(currentColour == COLOUR_RED){
        prevColourX = COLOUR_RED;
        //STATE 5 = FALLING EDGE
        state = STATE_5;
      }
      
    break;
    
    //GREEN BEHAVIOUR
    case STATE_2:
      //GOING OVER A GREEN LINE 
      if(currentColour == COLOUR_GREEN){
        prevColourX = COLOUR_GREEN;
        //STATE 5 = FALLING EDGE
        state = STATE_5;
      }

    break;

    //YELLOW BEHAVIOUR
    case STATE_3:
      //GOING OVER A YELLOW LINE 
      if(currentColour == COLOUR_YELLOW){
        prevColourY = COLOUR_YELLOW;
        //STATE 5 = FALLING EDGE
        state = STATE_5;
      }

    //BLUE BEHAVIOUR
    case STATE_4:
      //GOING OVER A BLUE LINE 
      if(currentColour == COLOUR_BLUE){
        prevColourY = COLOUR_BLUE;
        //STATE 5 = FALLING EDGE
        state = STATE_5;
      }
      
    break;
      
    //FALLING EDGE
    case STATE_5:
    //COMING OFF A RED LINE
    if((currentColour == COLOUR_BLACK) && (prevColourX == COLOUR_RED)){
      countX++;
      pathX[indexX] = COLOUR_RED;
      indexX++;
      prevColourX = COLOUR_BLACK;
      state = STATE_0;
    }
    //COMING OFF A GREEN LINE
    if((currentColour == COLOUR_BLACK) && (prevColourX == COLOUR_GREEN)){
      countX++;
      pathX[indexX] = COLOUR_GREEN;
      indexX++;
      prevColourX = COLOUR_BLACK;
      state = STATE_0;
    }
    //COMING OFF A YELLOW LINE
    if((currentColour == COLOUR_BLACK) && (prevColourY == COLOUR_YELLOW)){
      countY++;
      pathY[indexY] = COLOUR_YELLOW;
      indexY++;
      prevColourY = COLOUR_BLACK;
      state = STATE_0;
    }
    //COMING OFF A BLUE LINE
    if((currentColour == COLOUR_BLACK) && (prevColourY == COLOUR_BLUE)){
      countY++;
      pathY[indexY] = COLOUR_BLUE;
      indexY++;
      prevColourY = COLOUR_BLACK;
      state = STATE_0;
    }
          
    break;
   
  } //END OF FINITE STATE MACHINE

  //FOR WHEN IT GOES BACK ON ITSELF
  for(int iX=1; iX < indexX;iX++) {
    //if newest item in the array is the same as the last
    if(pathX[iX] == pathX[iX - 1]) {
      countX -=2;
      indexX -=2;
    }
  }
  for(int iY=1; iY < indexY;iY++) {
    //if newest item in the array is the same as the last
    if(pathY[iY] == pathY[iY - 1]) {
      countY -=2;
      indexY -=2;
    }
  }

  //PRINTING FOR DEBUGGING PURPOSES
  Serial.print("currentColour: ");
  Serial.print(currentColour);
  Serial.println("");
  
  Serial.print("state: ");
  Serial.print(state);
  Serial.println("");
  
  Serial.print("CountX: ");
  Serial.print(countX);
  Serial.println("");
  
  Serial.print("CountY: ");
  Serial.print(countY);
  Serial.println("");
  
  Serial.println("");
  
} //END OF MAIN LOOP

int getColour(){
    
  uint16_t r, g, b, c, colorTemp, lux;
  
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  
  //NORMALISE VALUES
  rNorm = (double)r/double(r + g + b);
  gNorm = (double)g/double(r + g + b);
  bNorm = (double)b/double(r + g + b);

  //CALCULATE EQUATION SECTIONS
  topA = 2 * rNorm - gNorm - bNorm;
  bottomA = sqrt(2.0 / 3.0);
  bottomB = sqrt(6 * (rNorm * rNorm + gNorm * gNorm + bNorm * bNorm - rNorm * gNorm - gNorm * bNorm - bNorm * rNorm));
  
  //CALCULATE H IN RADIANS
  if (bNorm <= gNorm) {
    h = acos(( topA / (bottomA * bottomB)));
    //CONVERT H TO DEGREES
    hDegrees = h * 180 / Pi;
  }
  if (bNorm > gNorm) {
    g = acos(( topA / (bottomA * bottomB)));
    h = twoPi - acos(( topA / (bottomA * bottomB)));
    //CONVERT H TO DEGREES
    hDegrees = h * 180 / Pi;
  }
    
    if((hDegrees>=330) && (hDegrees <= 0)){
      outputColour = COLOUR_RED;
      Serial.print("RED");
      Serial.println("");
      detected = true;
    }
    if((hDegrees >= 0) && (hDegrees <= 25)){
      outputColour = COLOUR_RED;
      Serial.print("RED");
      Serial.println("");
      detected = true;
    } else
    if((hDegrees >= 40) && (hDegrees <= 70)){
      outputColour = COLOUR_YELLOW;
      Serial.print("YELLOW");
      Serial.println("");
      detected = true;
    } else
    if((hDegrees >= 100) && (hDegrees <= 130)){
      outputColour = COLOUR_GREEN;
      Serial.print("GREEN");
      Serial.println("");
      detected = true;
    } else
    if((hDegrees >= 180) && (hDegrees <= 250)){
      outputColour = COLOUR_BLUE;
      Serial.print("BLUE");
      Serial.println("");
      detected = true;
    } else
    if(detected = true){
      outputColour = COLOUR_BLACK;
      Serial.print("BLACK");
      Serial.println("");
      detected = false;
    }
      
    return outputColour;
      
  } //END OF getColour()
