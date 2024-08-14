#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"

#define ShowSerial Serial

// Create a TFT_eSPI object
TFT_eSPI tft = TFT_eSPI();

bool loggingTimestamps = false;
File myFile;

// Function to initialize the SD card
void initSDCard() {
    if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
        tft.fillScreen(TFT_WHITE);
        tft.setCursor(0, 0);
        tft.setTextSize(2);
        tft.setTextColor(TFT_RED);
        tft.println("SD card initialization failed!");
        while (1);  // Stop execution if SD card initialization fails
    }
    tft.fillScreen(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK);
    tft.println("SD card initialization done.");
}

void setup() {
    Serial.begin(115200);

    // Initialize the display
    tft.init();
    tft.setRotation(3); 
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);

    // Initialize the button
    pinMode(WIO_KEY_C, INPUT_PULLUP);

    // Initialize SD card
    initSDCard();
}

void loop() {
    if (digitalRead(WIO_KEY_C) == LOW) {
        while (digitalRead(WIO_KEY_C) == LOW);  // Wait for button release
        loggingTimestamps = !loggingTimestamps;
        tft.fillScreen(TFT_WHITE);
        tft.setCursor(0, 0);
        tft.setTextSize(2);
        if (loggingTimestamps) {
            tft.println("Logging Timestamps");
        } else {
            tft.println("Stopped Logging");
        }
        delay(200);
    }

    if (loggingTimestamps) {
        myFile = SD.open("TimestampLog.txt", FILE_APPEND);
        if (myFile) {
            myFile.print("Timestamp: ");
            myFile.println(millis() / 1000);  // Log time in seconds
            myFile.close();
        } else {
            Serial.println("Error opening TimestampLog.txt");
        }
        delay(1000);  // Log every second
    }
}
