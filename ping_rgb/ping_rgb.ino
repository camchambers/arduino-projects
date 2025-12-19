#include "Arduino_LED_Matrix.h"

const int pingPin = 2;
// PWM Pins for RGB LED
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

ArduinoLEDMatrix matrix;
uint8_t frame[8][12] = {0};

void setup() {
  Serial.begin(115200);
  
  // Set RGB pins as outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  matrix.begin();
  Serial.println("READY");
}

void loop() {
  long duration, cm;

  // Trigger the Ping))) sensor
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5); 
  digitalWrite(pingPin, LOW);
  pinMode(pingPin, INPUT);
  
  duration = pulseIn(pingPin, HIGH, 30000); 

  if (duration == 0) {
    drawError();
    setRGB(0, 0, 0); // Turn LED off on error
  } else {
    cm = duration / 29 / 2;
    
    updateMatrix(cm);
    updateRGB(cm); // New function to handle the LED color
    
    Serial.print("Distance: ");
    Serial.print(cm);
    Serial.println(" cm");
  }

  delay(50); 
}

// Logic to change LED color based on distance
void updateRGB(int distance) {
  int r = 0, g = 0, b = 0;

  // Constrain distance to our working range for stable math
  int d = constrain(distance, 5, 40);

  if (d <= 20) {
    // ZONE 1: Transition Red to Green
    // As distance increases from 5 to 20:
    // Red goes 255 -> 0, Green goes 0 -> 255
    r = map(d, 5, 20, 255, 0);
    g = map(d, 5, 20, 0, 255);
    b = 0;
  } 
  else {
    // ZONE 2: Transition Green to Blue
    // As distance increases from 20 to 40:
    // Green goes 255 -> 0, Blue goes 0 -> 255
    r = 0;
    g = map(d, 20, 40, 255, 0);
    b = map(d, 20, 40, 0, 255);
  }

  setRGB(r, g, b);
}

// Helper to handle Common Anode logic (V-B-R-G)
void setRGB(int r, int g, int b) {
  // We subtract from 255 because Common Anode is "Active Low"
  analogWrite(redPin, 255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin, 255 - b);
}

void updateMatrix(int distance) {
  int columnsToLight = map(distance, 5, 40, 12, 0);
  columnsToLight = constrain(columnsToLight, 0, 12);

  memset(frame, 0, sizeof(frame));
  for (int col = 0; col < columnsToLight; col++) {
    for (int row = 0; row < 8; row++) {
      frame[row][col] = 1;
    }
  }
  matrix.renderBitmap(frame, 8, 12);
}

void drawError() {
  memset(frame, 0, sizeof(frame));
  frame[3][5] = 1; frame[3][6] = 1;
  frame[4][5] = 1; frame[4][6] = 1;
  matrix.renderBitmap(frame, 8, 12);
}