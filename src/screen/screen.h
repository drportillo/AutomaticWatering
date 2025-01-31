#ifndef SCREEN_H
#define SCREEN_H

//Function to initialize the screen
void startScreen();

// Function to render the screen with sensor data
void renderScreen(float temperatures[], float humidities[], bool wifi_status, bool blt_status);

// Function to draw connectivity indicators (circles)
void drawSensorIndicators(float temperatures[]);

void drawWifiIndicator(bool status); 

void drawBLTIndicator(bool status); 
#endif