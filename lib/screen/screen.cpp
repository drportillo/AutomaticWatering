#include "Arduino.h"
#include <TFT_eSPI.h>
#include "screen.h"
#include "constants.h"

#define SCREEN_W      240
#define SCREEN_H      240
#define TOP_BAR_H      20
#define COL_W          80
#define FLOWER_AREA_H 110
#define SENSOR_ROW_H   55

// y=20..129 flower, y=130..184 temp, y=185..239 hum  (20+110+55+55=240)
#define ROW_FLOWER_Y  (TOP_BAR_H)
#define ROW_TEMP_Y    (TOP_BAR_H + FLOWER_AREA_H)
#define ROW_HUM_Y     (TOP_BAR_H + FLOWER_AREA_H + SENSOR_ROW_H)

TFT_eSPI tft = TFT_eSPI();

static void drawLayout() {
    tft.drawFastHLine(0, TOP_BAR_H,  SCREEN_W, TFT_WHITE);
    tft.drawFastHLine(0, ROW_TEMP_Y, SCREEN_W, TFT_WHITE);
    tft.drawFastHLine(0, ROW_HUM_Y,  SCREEN_W, TFT_WHITE);
    tft.drawFastVLine(COL_W,     TOP_BAR_H, SCREEN_H - TOP_BAR_H, TFT_WHITE);
    tft.drawFastVLine(COL_W * 2, TOP_BAR_H, SCREEN_H - TOP_BAR_H, TFT_WHITE);
}

static int centeredX(int col_x, const char* str, int s) {
    return col_x + (COL_W - (int)strlen(str) * 6 * s) / 2;
}

static void drawCell(int col_x, int row_y, const char* label, const char* value, uint16_t valueColor) {
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(centeredX(col_x, label, 1), row_y + 4);
    tft.print(label);

    tft.setTextSize(3);
    tft.setTextColor(valueColor, TFT_BLACK);
    tft.setCursor(centeredX(col_x, value, 3), row_y + 16);
    tft.print(value);
}

enum Mood { HAPPY, NEUTRAL, SAD, FACE_OFF };

static Mood getMood(float humidity, bool connected) {
    if (!connected) return FACE_OFF;
    if (humidity >= 60) return HAPPY;
    if (humidity >= 30) return NEUTRAL;
    return SAD;
}

static uint16_t faceColor(float temperature) {
    if (temperature <= 10.0f) return TFT_BLUE;
    if (temperature <= 25.0f) return 0xFFE0;  // yellow
    return TFT_RED;
}

static void drawFace(int cx, int cy, Mood mood, float temperature) {
    tft.fillCircle(cx, cy, 18, faceColor(temperature));
    tft.drawCircle(cx, cy, 18, TFT_BLACK);

    if (mood == FACE_OFF) {
        // X eyes
        tft.drawLine(cx - 8, cy - 7, cx - 4, cy - 3, TFT_BLACK);
        tft.drawLine(cx - 4, cy - 7, cx - 8, cy - 3, TFT_BLACK);
        tft.drawLine(cx + 4, cy - 7, cx + 8, cy - 3, TFT_BLACK);
        tft.drawLine(cx + 8, cy - 7, cx + 4, cy - 3, TFT_BLACK);
        // flat mouth
        tft.fillRect(cx - 5, cy + 6, 10, 2, TFT_BLACK);
        return;
    }

    // Normal round eyes
    tft.fillCircle(cx - 6, cy - 5, 2, TFT_BLACK);
    tft.fillCircle(cx + 6, cy - 5, 2, TFT_BLACK);

    if (mood == HAPPY) {
        // Smile: corners high (small y), center low (large y)
        for (int x = -6; x <= 6; x++) {
            int y = -(x * x) / 9 + 9;
            tft.fillRect(cx + x, cy + y, 2, 2, TFT_BLACK);
        }
    } else if (mood == SAD) {
        // Frown: corners low (large y), center high (small y)
        for (int x = -6; x <= 6; x++) {
            int y = (x * x) / 9 + 3;
            tft.fillRect(cx + x, cy + y, 2, 2, TFT_BLACK);
        }
    } else {
        // Neutral: flat line
        tft.fillRect(cx - 5, cy + 6, 10, 2, TFT_BLACK);
    }
}

// Teardrop at (x, y): pointed top, round bottom — total height ~15px
static void drawDroplet(int x, int y) {
    tft.fillTriangle(x, y - 7, x - 5, y + 2, x + 5, y + 2, TFT_CYAN);
    tft.fillCircle(x, y + 2, 5, TFT_CYAN);
}

static void drawFlower(int col_x, float humidity, float temperature, bool connected, bool pump) {
    int cx = col_x + COL_W / 2;
    int cy = ROW_FLOWER_Y + FLOWER_AREA_H / 2;  // = 75

    Mood mood = getMood(humidity, connected);

    // 8 petals at 45° intervals
    for (int p = 0; p < 8; p++) {
        float angle = p * PI / 4.0f;
        int px = cx + (int)(22.0f * cosf(angle));
        int py = cy + (int)(22.0f * sinf(angle));
        tft.fillCircle(px, py, 11, TFT_WHITE);
    }

    drawFace(cx, cy, mood, temperature);

    // Water droplets in the gap above the flower when pump is running.
    // Top petal edge ≈ cy-33; zone top = ROW_FLOWER_Y. 20px of headroom.
    // Droplet is 15px tall (y-7 to y+8), so center at cy-42 keeps bottom 9px clear of petal.
    if (pump) {
        int dy = cy - 42;
        drawDroplet(cx - 13, dy + 5);
        drawDroplet(cx,       dy);
        drawDroplet(cx + 13, dy + 5);
    }
}

void startScreen() {
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    drawLayout();

    tft.setTextSize(1);
    tft.setCursor(4, 6);
    tft.print("WiFi:--   BT:--");

    for (int i = 0; i < 3; i++) {
        drawFlower(i * COL_W, 50.0f, 20.0f, true, false);  // neutral placeholder
    }
}

void renderScreen(float temperatures[], float humidities[], bool pumps[], bool wifi_status, bool blt_status) {
    tft.fillScreen(TFT_BLACK);
    drawLayout();

    // Top bar
    tft.setTextSize(1);
    tft.setCursor(4, 6);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("WiFi:");
    tft.setTextColor(wifi_status ? TFT_GREEN : TFT_RED, TFT_BLACK);
    tft.print(wifi_status ? "ON " : "OFF");

    tft.setCursor(140, 6);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("BT:");
    tft.setTextColor(blt_status ? TFT_GREEN : TFT_RED, TFT_BLACK);
    tft.print(blt_status ? "ON " : "OFF");

    for (int i = 0; i < 3; i++) {
        int col_x = i * COL_W;
        bool connected = temperatures[i] >= DISCONNECTED_TEMP_THRESHOLD;

        drawFlower(col_x, humidities[i], temperatures[i], connected, pumps[i]);

        char tempStr[8];
        if (connected) dtostrf(temperatures[i], 1, 1, tempStr);
        else strcpy(tempStr, "---");
        drawCell(col_x, ROW_TEMP_Y, "TEMP", tempStr, TFT_WHITE);

        char humStr[6];
        if (connected) sprintf(humStr, "%d%%", (int)humidities[i]);
        else strcpy(humStr, "---");
        drawCell(col_x, ROW_HUM_Y, "HUM", humStr, TFT_WHITE);
    }

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}
