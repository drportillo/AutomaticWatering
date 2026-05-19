#ifndef PUMPS_H
#define PUMPS_H

void initPumps();
void updatePumps(float humidities[]);
void setPump(int pin, bool state);

#endif
