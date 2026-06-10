#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <math.h>

// --- CONFIG ---
#define CALIBRATE_BUTTON 6  // The button to press (pin 6)
#define BNO_ADDR 0x28
// --- END CONFIG ---

Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO_ADDR);

const char* zoneNames[5] = {"snare", "hihat", "tom", "crash", "ride"};
int currentZone = 0;
bool isBnoCalibrated = false;

// Array to store the Yaw/Pitch for all 5 zones
float calibratedValues[5][2]; // [zone][0=yaw, 1=pitch]

// Debounce timer
unsigned long lastPress = 0;
#define DEBOUNCE_DELAY 500 // 0.5 second delay between presses

void setup() {
  Serial.begin(230400);
  pinMode(CALIBRATE_BUTTON, INPUT_PULLUP);
  Wire.setClock(400000);

  if (!bno.begin()) {
    Serial.println("BNO055 not detected! Check wiring.");
    while(1); // Stop
  }
  bno.setExtCrystalUse(true);

  Serial.println("\n--- Drumstick Calibration Tool ---");
  Serial.println("Part 1: Calibrate the BNO055 Sensor.");
  Serial.println("Move the sensor in a figure-8 pattern...");
  Serial.println("Waiting for full system calibration (sys=3)...");
}

void loop() {
  
  // --- PART 1: SENSOR CALIBRATION ---
  if (!isBnoCalibrated) {
    uint8_t sys, gyro, accel, mag;
    sys = gyro = accel = mag = 0;
    bno.getCalibration(&sys, &gyro, &accel, &mag);

    Serial.print("Calib Status: sys="); Serial.print(sys);
    Serial.print(" gyro="); Serial.print(gyro);
    Serial.print(" accel="); Serial.print(accel);
    Serial.print(" mag="); Serial.println(mag);

    if (sys == 3) {
      Serial.println("\n--- SENSOR CALIBRATED! ---");
      Serial.println("\nPart 2: Calibrate Drum Positions.");
      isBnoCalibrated = true;
      promptNextZone(); // Print the first prompt
    }
    
    delay(200); 
    return; 
  }

  // --- PART 2: DRUM POSITION CALIBRATION ---

  if (currentZone >= 5) {
    
    if (currentZone == 5) { // Only run this once
        Serial.println("\n--- CALIBRATION COMPLETE ---");
        Serial.println("Copy the block below and paste it into your main code:");
        Serial.println("---------------------------------");
        
        // Loop through all 5 zones and print the stored values
        for (int i = 0; i < 5; i++) {
            Serial.print("DrumZone ");
            Serial.print(zoneNames[i]);
            
            // Add padding spaces for alignment
            int padding = 6 - strlen(zoneNames[i]);
            for (int p = 0; p < padding; p++) {
              Serial.print(" ");
            }
            
            Serial.print(" = {");
            Serial.print(calibratedValues[i][0], 1); // Stored Yaw
            Serial.print(", ");
            Serial.print(calibratedValues[i][1], 1); // Stored Pitch
            Serial.println("};");
        }
        Serial.println("---------------------------------");
        
        currentZone++; // Increment so this message doesn't repeat
    }
    return; // Stop
  }

  // Check for the button press
  if (digitalRead(CALIBRATE_BUTTON) == LOW && (millis() - lastPress > DEBOUNCE_DELAY)) {
    lastPress = millis(); // Reset debounce timer

    sensors_event_t event;
    if (bno.getEvent(&event, Adafruit_BNO055::VECTOR_EULER)) {
      
      // Store values in the array
      calibratedValues[currentZone][0] = event.orientation.x; // Save Yaw
      calibratedValues[currentZone][1] = event.orientation.y; // Save Pitch

      // Print a simple confirmation for the user
      Serial.print("...Saved [");
      Serial.print(zoneNames[currentZone]);
      Serial.print("]: {");
      Serial.print(calibratedValues[currentZone][0], 1);
      Serial.print(", ");
      Serial.print(calibratedValues[currentZone][1], 1);
      Serial.println("}");
      
      // Move to the next zone
      currentZone++;
      promptNextZone(); // Print the next prompt

    } else {
      Serial.println("Error reading BNO! Please try again.");
    }
  }
}

/**
 * Prints the prompt for the next zone to calibrate.
 */
void promptNextZone() {
  if (currentZone < 5) {
    Serial.print("\n>>> Point the stick at the [");
    Serial.print(zoneNames[currentZone]);
    Serial.println("]");
    Serial.println("...then press the button to save.");
  }
}
