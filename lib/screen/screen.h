#ifndef SCREEN_H
#define SCREEN_H

void startScreen();
void renderScreen(float temperatures[], float humidities[], bool pumps[], bool wifi_status, bool blt_status, const char* timeStr);

#endif
