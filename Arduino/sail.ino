//  _ ___ _______     ___ ___ ___  ___ _   _ ___ _____ ___ 
// / |_  )__ /   \   / __|_ _| _ \/ __| | | |_ _|_   _/ __| 
// | |/ / |_ \ |) | | (__ | ||   / (__| |_| || |  | | \__ \ 
// |_/___|___/___/   \___|___|_|_\\___|\___/|___| |_| |___/ 
// 
// Button Debounce and Signal Conditioning 
// 
// Made by agvesperman agvesperman
// License: CC-BY-SA 3.0
// Downloaded from: https://circuits.io/circuits/3642099-button-debounce-and-signal-conditioning
// Remove all comments with four slash for use on real hardware feather M0
/*
    Sail Lighting Project
    
    This code demonstrates how to use a signal generator to test the button debounce 
    and signal conditioning logic.

*/

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define BUTTON_OFF 30
#define BUTTON_ON 100
#define BUTTON_OFF_HOLD_DURATION 300
#define BUTTON_DEBOUNCE_DURATION 1

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

int buttonA = A0; 
int buttonB = A1; 
int buttonC = A2; 
int buttonD = A3; 
int valA = 0;
int valB = 0;
int valC = 0;
int valD = 0;


unsigned long time = 0;
unsigned long last_loop_time = 0;

int current_state = 0;
int counter = 0;
int reading = 0;
int button_releasing_duration = 0;
unsigned long button_released_time = 0;
int button_pressing_duration = 0;
unsigned long button_pressed_time = 0;
int show_debug = 0;

bool prev_UnpressedA = false;
bool prev_UnpressedB = false;
bool on = true;
int array_spot = 0;
int OFF = 1;
int RED = 2;
int BLUE = 3;
int PURPLE = 4;
int GREEN = 5;
int YELLOW = 6;
int colors[] = {OFF, // Me, You will be my mom...
                RED, // A red one
                RED, // A blue one
                OFF, // The red one came back
                OFF, // The blue one came back
                RED, // I want to change the world,instead I sleep...
                OFF, // Now
                RED, // Red
                OFF, // I hear
                BLUE, // Blue
                OFF, // I feel 
                PURPLE,// Purple
                OFF, // I taste
                GREEN, // Green 
                YELLOW,// I choose yellow
                OFF, // Land ho!
                RED, // The storm is coming but i don't mind...
                OFF,// Now
                255};
int colors2[] = {OFF, // Me, You will be my mom...
                OFF, // A red one
                BLUE, // A blue one
                BLUE, 
                OFF,
                BLUE,
                OFF,
                RED,
                OFF,
                BLUE,
                OFF,
                PURPLE,
                OFF,
                GREEN,
                OFF,
                YELLOW,
                OFF,
                BLUE,
                OFF,
                 255};
int colors3[] = {OFF, // Me, You will be my mom...
                RED, // A red one
                RED, // A blue one
                OFF, 
                1, 2, 1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 2, 1,255};

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif



// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(buttonA, INPUT);
  Serial.begin(9600);
  Serial.println("Loaded");
  pixels.begin(); // This initializes the NeoPixel library.
   for(int pixelNumber=0;pixelNumber<NUMPIXELS;pixelNumber++){
  pixels.setPixelColor(pixelNumber, 0,255,0);
  }
  pixels.show();
  button_pressed_time = millis();
  button_released_time = millis();
}

void getColor(int n)
{
  for(int pixelNumber=0;pixelNumber<NUMPIXELS;pixelNumber++) {
    if ((n==1) || (n==255)) {
      pixels.setPixelColor(pixelNumber, 0,0,0);
      digitalWrite(led, LOW);
      on = false;
      Serial.println("setPixelColor = Off");
    } else if (n==2) {
      pixels.setPixelColor(pixelNumber, 255,0,0);
            Serial.println("setPixelColor = RED");
    } else if (n==3) {
      pixels.setPixelColor(pixelNumber, 0,0,255);
      Serial.println("setPixelColor = BLUE");
    } else if (n==4) {
      pixels.setPixelColor(pixelNumber, 160,32,240);
      Serial.println("setPixelColor = PURPLE");
    } else if (n==5) {
      pixels.setPixelColor(pixelNumber, 0,255,0);
      Serial.println("setPixelColor = GREEN");
    } else if (n==6) {
      pixels.setPixelColor(pixelNumber, 255,255,0);
      Serial.println("setPixelColor = YELLOW");
    }
  }
}
  
int debounceInput(int reading)
{
  // If we have gone on to the next millisecond
  if(millis() != time) {
    if(reading == current_state && counter > 0) {
      counter--;
    }
    if(reading != current_state) {
       counter++; 
    }
    // If the Input has shown the same value for long enough let's switch it
    if(counter >= BUTTON_DEBOUNCE_DURATION) {
      counter = 0;
      current_state = reading;  
    }
    time = millis();   
  }
  return current_state;
}





void scanInputs()
{
  if (Serial.available() > 0) {
    show_debug = Serial.read();
  }
  valA = analogRead(buttonA);
  if (show_debug > 0) {
    Serial.print("Raw=");
    Serial.print(valA,DEC);
  }
  valA = debounceInput(valA);
  if (show_debug > 0) {
    Serial.print(",Debounced=");
    Serial.println(valA,DEC);
  }
  if (valA > BUTTON_ON) {
    if(prev_UnpressedA) {              
      getColor(colors[array_spot]);
      digitalWrite(led, HIGH);
      Serial.println("onButtonPressed");
      button_pressed_time = millis();
      button_pressing_duration = 0;
      button_releasing_duration = 0;
      prev_UnpressedA = false;
      if(colors[array_spot] != 255) {
        array_spot++;
      }
      Serial.print("Next array_spot=");
      Serial.println(array_spot);
      Serial.print("Next colors[array_spot]=");
      Serial.println(colors[array_spot]);
    } else {
      button_pressing_duration = millis()-button_pressed_time;
    }
    Serial.print(button_pressing_duration);
    Serial.println("ms pressed ");   
  } else if(valA <= BUTTON_OFF) {
    button_releasing_duration = millis()-(button_pressed_time+button_pressing_duration);
    if (!prev_UnpressedA) {
      if (button_releasing_duration > BUTTON_OFF_HOLD_DURATION) {
        prev_UnpressedA = true;
        digitalWrite(led, LOW);
        Serial.println("onButtonReleased");
        button_released_time = millis();
      } else {
        Serial.print(button_releasing_duration);
        Serial.println("ms releasing (or low battery voltage dip?)");
      }
    } else {
      Serial.print(millis()-button_released_time);
      Serial.println("ms released");   
    }
    
      

  } else {
   Serial.println("Not ON or OFF");
  }
  pixels.show();
}

// the loop routine runs over and over again forever:
void loop() 
{
  if (millis() != last_loop_time){
    last_loop_time = millis();
    scanInputs();
  } else {
    // skip this loop and wait for the next millisecond
    return;
  }
}    
