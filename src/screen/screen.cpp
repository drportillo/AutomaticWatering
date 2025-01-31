#include "Arduino.h"
#include <TFT_eSPI.h>  // Include the TFT_eSPI library
#include "screen.h"
#include "../constants.h"

// Initialize the TFT display
TFT_eSPI tft = TFT_eSPI();

void startScreen() {
  // Initialize the TFT display
  tft.init();
  tft.setRotation(0);         // Set rotation to 0 for your display
  tft.fillScreen(TFT_BLACK);  // Clear the screen with black background

  // Set text color and size
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);  // Larger text size for better readability

  // Display initial message
  tft.setCursor(5, 5);
  tft.print("WiFi");
  tft.println(" - ");
  tft.setCursor(50, 5);
  tft.println("BLT");

  for (int i = 0; i < 3; i++) {
    int x = CIRCLE_X_START + i * (CIRCLE_RADIUS * 2 + CIRCLE_SPACING);
    int y = CIRCLE_Y;

    tft.fillCircle(x, y, CIRCLE_RADIUS, TFT_WHITE);
  }


  tft.setTextSize(2.5);
  // Display initial message
  tft.setCursor(5, 50);
  tft.println("Initializing sensors...");
}

// Function to render the screen with sensor data
void renderScreen(float temperatures[], float humidities[], bool wifi_status = true, bool blt_status = true) {
  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Draw visual indicators for sensor connectivity
  drawWifiIndicator(wifi_status);
  drawBLTIndicator(blt_status);
  drawSensorIndicators(temperatures);

  // Display each pair of sensor readings one by one
  for (int i = 0; i < 3; i++) {
    // Skip this sensor pair if the temperature is below the threshold (sensor disconnected)
    if (temperatures[i] < DISCONNECTED_TEMP_THRESHOLD) {
      continue;  // Skip to the next sensor pair
    }

    // Display the current sensor pair
    tft.setCursor(10, 50);
    tft.print("T");
    tft.print(i + 1);
    tft.print(": ");
    tft.print(temperatures[i]);
    tft.println(" C");

    tft.setCursor(10, 100);
    tft.print("H");
    tft.print(i + 1);
    tft.print(": ");
    tft.print(humidities[i]);
    tft.println(" %");

    // Wait for 2 seconds before showing the next pair
    delay(2000);
  }
}

// Function to draw connectivity indicators (circles)
void drawSensorIndicators(float temperatures[]) {
  for (int i = 0; i < 3; i++) {
    int x = CIRCLE_X_START + i * (CIRCLE_RADIUS * 2 + CIRCLE_SPACING);
    int y = CIRCLE_Y;

    // Set circle color based on sensor connectivity
    if (temperatures[i] < DISCONNECTED_TEMP_THRESHOLD) {
      tft.fillCircle(x, y, CIRCLE_RADIUS, TFT_RED);  // Red for disconnected
    } else {
      tft.fillCircle(x, y, CIRCLE_RADIUS, TFT_GREEN);  // Green for connected
    }
  }
}


// Function to draw connectivity indicators (circles)
void drawWifiIndicator(bool wifi_status) {

  tft.setTextSize(1);

  // Set circle color based on sensor connectivity
  if (wifi_status) {
    // Set text color
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  } else {
    // Set text color
    tft.setTextColor(TFT_RED, TFT_BLACK);
  };

  // Display initial message
  tft.setCursor(5, 5);
  tft.print("WiFi");


  // Set text color and size
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(" - ");
  tft.setTextSize(2);  // Larger text size for better readability
}


// Function to draw connectivity indicators (circles)
void drawBLTIndicator(bool BLT_status) {

  tft.setTextSize(1);

  // Set circle color based on sensor connectivity
  if (BLT_status) {
    // Set text color
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  } else {
    // Set text color
    tft.setTextColor(TFT_RED, TFT_BLACK);
  };

  // Display initial message
  tft.setCursor(50, 5);
  tft.println("BLT");


  // Set text color and size
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);  // Larger text size for better readability
}