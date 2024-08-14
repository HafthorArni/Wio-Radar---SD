#include <SPI.h>
#include <Wire.h>
#include <TFT_eSPI.h>  // Include the graphics library for the Wio Terminal
#include "60ghzbreathheart.h"
#include "seeed_line_chart.h"  // Include the line chart library

#define ShowSerial Serial
BreathHeart_60GHz radar = BreathHeart_60GHz(&Serial1);

int heart_rate = 0;
int previous_heart_rate = -1;
int breath_rate = 0;
int previous_breath_rate = -1;
int residency = 1;
int previous_residency = -1;
int resident_movement = 0;
int previous_resident_movement = -1;
float Resident_distance = 0.71;
float previous_Resident_distance = -1;
float direction_x = 0;
float direction_y = 0;
float direction_z = 0;
float previous_direction_x = -3;
float previous_direction_y = -3;
float previous_direction_z = -3;
int residency_fall = 1;
int previous_residency_fall = -1;
int fall_status = 0;
int previous_fall_status = -1;
bool previousInBed = -1;
int previousAwakeTime = -1;
int previousLightTime = -1;
int previousDeepTime = -1;
int previousSleepScore = -1;
int previousTurnNum = -1;
int previousApneaNum = -1;
int previousBreathRate = -1;
int previousHeartRate = -1;
bool inMenu = false;
int lastDisplayModeBeforeMenu = -1;


int display_mode = 0;  // 0: heart and breath rates, 1: human existence, 2: bodysign_val graph, 3: heart rate graph

#define MAX_SIZE 30  // Maximum size of data for the graph
doubles bodysign_data;  // Initialize a doubles type to store bodysign data
doubles heart_data;  // Initialize a doubles type to store heart rate data
doubles breath_data;  // Initialize a doubles type to store breath rate data

// Create a TFT_eSPI object
TFT_eSPI tft = TFT_eSPI();

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    // Initialize the display
    tft.init();
    tft.setRotation(3);  // Adjust as necessary for your setup
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);

    // Button setup
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_5S_UP, INPUT_PULLUP);
    pinMode(WIO_5S_DOWN, INPUT_PULLUP);
    pinMode(WIO_5S_LEFT, INPUT_PULLUP);
    pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);

    tft.setCursor(0, 0);
    tft.println("Initializing...");
    delay(1000);
    radar.reset_func();
    delay(50);

    radar.ModeSelect_fuc(1);

    tft.fillScreen(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.println("Radar Heartbeat");
    tft.setCursor(0, 16);
    tft.println("Sensor Initialization Done");
    delay(2000);
    Serial.println("System initialized and radar set to real-time mode.");
}

void resetValues() {
    heart_rate = 0;
    previous_heart_rate = -1;
    breath_rate = 0;
    previous_breath_rate = -1;
    residency = 0;
    previous_residency = -1;
    resident_movement = 0;
    previous_resident_movement = -1;
    Resident_distance = 0;
    previous_Resident_distance = -1;
    direction_x = 0;
    direction_y = 0;
    direction_z = 0;
    previous_direction_x = -3;
    previous_direction_y = -3;
    previous_direction_z = -3;
    residency_fall = 1;
    previous_residency_fall = -1;
    fall_status = 0;
    previous_fall_status = -1;
    previousInBed = -1;
    previousAwakeTime = -1;
    previousLightTime = -1;
    previousDeepTime = -1;
    previousSleepScore = -1;
    previousTurnNum = -1;
    previousApneaNum = -1;
    previousBreathRate = -1;
    previousHeartRate = -1;


}

void displayHeartAndBreathRates() {
    radar.Breath_Heart();

    tft.setCursor(20, 80);
    tft.setTextSize(3);
    tft.print("Heart Rate: ");
    tft.setCursor(20, 120);
    tft.print("Breath Rate: ");

    if (radar.sensor_report != 0x00) {
        if (radar.sensor_report == HEARTRATEVAL) {
            heart_rate = radar.heart_rate;
        } else if (radar.sensor_report == BREATHVAL) {
            breath_rate = radar.breath_rate;
        }
    }

    // Update heart rate if different
    if (heart_rate != previous_heart_rate) {
        // Clear previous heart rate value
        tft.fillRect(250, 80, 100, 30, TFT_WHITE);
        tft.setCursor(250, 80);
        tft.setTextSize(3);
        tft.print(heart_rate);
        previous_heart_rate = heart_rate;
    }

    // Update breath rate if different
    if (breath_rate != previous_breath_rate) {
        // Clear previous breath rate value
        tft.fillRect(250, 120, 100, 30, TFT_WHITE);
        tft.setCursor(250, 120);
        tft.setTextSize(3);
        tft.print(breath_rate);
        previous_breath_rate = breath_rate;
    }
    delay(200);
}

void displayHumanExistence() {
    radar.HumanExis_Func();

    if (radar.sensor_report != 0x00) {
        switch (radar.sensor_report) {
            case NOONE:
                Serial.println("Nobody here.");
                Serial.println("----------------------------");
                residency = 0;
                break;
            case SOMEONE:
                Serial.println("Someone is here.");
                Serial.println("----------------------------");
                residency = 1;
                break;
            case NONEPSE:
                Serial.println("No human activity messages.");
                Serial.println("----------------------------");
                break;
            case STATION:
                Serial.println("Someone stop");
                Serial.println("----------------------------");
                resident_movement = 0;
                break;
            case MOVE:
                Serial.println("Someone moving");
                Serial.println("----------------------------");
                resident_movement = 1;
                residency = 1;
                break;
            case BODYVAL:
                // Serial.print("The parameters of human body signs are: ");
                // Serial.println(radar.bodysign_val, DEC);
                // Serial.println("----------------------------");
                break;
            case DISVAL:
                Serial.print("The sensor judges the distance to the human body to be: ");
                Serial.print(radar.distance, DEC);
                Serial.println(" m");
                Serial.println("----------------------------");
                Resident_distance = radar.distance;
                break;
            case DIREVAL:
                Serial.print("The sensor judges the orientation data with the human body as -- x: ");
                Serial.print(radar.Dir_x);
                Serial.print(" m, y: ");
                Serial.print(radar.Dir_y);
                Serial.print(" m, z: ");
                Serial.print(radar.Dir_z);
                Serial.println(" m");
                Serial.println("----------------------------");
                direction_x = radar.Dir_x;
                direction_y = radar.Dir_y;
                direction_z = radar.Dir_z;
                break;
        }
    }

    tft.setCursor(20, 50);
    tft.setTextSize(3);
    tft.print("Residency: ");
    tft.setCursor(20, 90);
    tft.print("Movement: ");
    tft.setCursor(20, 130);
    tft.print("Distance: ");
    tft.setCursor(10, 170);
    tft.print("X: ");
    tft.setCursor(110, 170);
    tft.print("Y: ");
    tft.setCursor(220, 170);
    tft.print("Z: ");
    // update status if different
    if (residency != previous_residency) {
        // Clear previous residency value
        tft.fillRect(220, 50, 100, 30, TFT_WHITE);
        tft.setCursor(220, 50);
        tft.setTextSize(3);
        tft.print(residency);
        previous_residency = residency;
    }
    if (resident_movement != previous_resident_movement) {
        // Clear previous fall status value
        tft.fillRect(220, 90, 100, 30, TFT_WHITE);
        tft.setCursor(220, 90);
        tft.setTextSize(3);
        tft.print(resident_movement);
        previous_resident_movement = resident_movement;
    }
    if (Resident_distance != previous_Resident_distance) {
        // Clear previous fall status value
        tft.fillRect(220, 130, 100, 30, TFT_WHITE);
        tft.setCursor(220, 130);
        tft.setTextSize(3);
        tft.print(Resident_distance);
        previous_Resident_distance = Resident_distance;
    }
    if (direction_x != previous_direction_x) {
        // Clear previous fall status value
        tft.fillRect(45, 175, 65, 30, TFT_WHITE);
        tft.setCursor(45, 175);
        tft.setTextSize(2);
        tft.print(direction_x);
        previous_direction_x = direction_x;
    }
    if (direction_y != previous_direction_y) {
        // Clear previous fall status value
        tft.fillRect(150, 175, 70, 30, TFT_WHITE);
        tft.setCursor(150, 175);
        tft.setTextSize(2);
        tft.print(direction_y);
        previous_direction_y = direction_y;
    }
    if (direction_z != previous_direction_z && direction_z != 0) {
        // Clear previous fall status value
        tft.fillRect(250, 175, 80, 30, TFT_WHITE);
        tft.setCursor(250, 175);
        tft.setTextSize(2);
        tft.print(direction_z);
        previous_direction_z = direction_z;
    }

    delay(200);
}

void displayBodysignGraph() {
    radar.HumanExis_Func();

    if (radar.sensor_report == BODYVAL) {
        int bodysign_val = radar.bodysign_val;

        if (bodysign_data.size() > MAX_SIZE) {
            bodysign_data.pop();
        }

        bodysign_data.push(bodysign_val);

        // Settings for the line graph title
        auto header = text(0, 0)
                          .value("Movement Readings")
                          .align(center)
                          .valign(vcenter)
                          .width(tft.width())
                          .thickness(2);

        header.height(header.font_height(&tft) * 2);
        header.draw(&tft); // Header height is the twice the height of the font

        // Settings for the line graph
        auto content = line_chart(20, header.height()); //(x,y) where the line graph begins
        content
            .height(tft.height() - header.height() * 1.5) // actual height of the line chart
            .width(tft.width() - content.x() * 2)         // actual width of the line chart
            .based_on(0.0)                                // Starting point of y-axis, must be a float
            .show_circle(true)                            // drawing a circle at each point, default is on.
            .value(bodysign_data)                         // passing through the data to line graph
            .max_size(MAX_SIZE)
            .color(TFT_RED)                               // Setting the color for the line
            .backgroud(TFT_WHITE)                         // Setting the color for the background
            .draw(&tft);
    }
    delay(50);
}

void displayHeartRateGraph() {
    radar.Breath_Heart();

    if (radar.sensor_report == HEARTRATEWAVE) {
        unsigned int heart_wave_vals[] = {
            radar.heart_point_1, radar.heart_point_2, radar.heart_point_3,
            radar.heart_point_4, radar.heart_point_5
        };
    

        // Settings for the line graph title
        auto header = text(0, 0)
                          .value("Heart Rate Waveform")
                          .align(center)
                          .valign(vcenter)
                          .width(tft.width())
                          .thickness(2);

        header.height(header.font_height(&tft) * 2);
        header.draw(&tft); // Header height is twice the height of the font

        for (int i = 0; i < 5; ++i) {
            if (heart_data.size() > MAX_SIZE) {
                heart_data.pop();
            }
            heart_data.push(heart_wave_vals[i]);

            // Settings for the line graph
            auto content = line_chart(20, header.height()); //(x,y) where the line graph begins
            content
                .height(tft.height() - header.height() * 1.5) // actual height of the line chart
                .width(tft.width() - content.x() * 2)         // actual width of the line chart
                .based_on(0.0)                                // Starting point of y-axis, must be a float
                .show_circle(true)                            // drawing a circle at each point, default is on.
                .value(heart_data)                            // passing through the data to line graph
                .max_size(MAX_SIZE)
                .color(TFT_BLUE)                              // Setting the color for the line
                .backgroud(TFT_WHITE)                         // Setting the color for the background
                .draw(&tft);
            //delay(190);
        }
    }
    delay(50);
}

void displayBreathWaveform() {
    radar.Breath_Heart();

    if (radar.sensor_report == BREATHWAVE) {
        unsigned int breath_wave_vals[] = {
            radar.breath_point_1, radar.breath_point_2, radar.breath_point_3,
            radar.breath_point_4, radar.breath_point_5
        };

        // Settings for the line graph title
        auto header = text(0, 0)
                          .value("Breath Waveform")
                          .align(center)
                          .valign(vcenter)
                          .width(tft.width())
                          .thickness(2);

        header.height(header.font_height(&tft) * 2);
        header.draw(&tft); // Header height is twice the height of the font

        for (int i = 0; i < 5; ++i) {
            if (breath_data.size() > MAX_SIZE) {
                breath_data.pop();
            }
            breath_data.push(breath_wave_vals[i]);

            // Settings for the line graph
            auto content = line_chart(20, header.height()); //(x,y) where the line graph begins
            content
                .height(tft.height() - header.height() * 1.5) // actual height of the line chart
                .width(tft.width() - content.x() * 2)         // actual width of the line chart
                .based_on(0.0)                                // Starting point of y-axis, must be a float
                .show_circle(true)                            // drawing a circle at each point, default is on.
                .value(breath_data)                           // passing through the data to line graph
                .max_size(MAX_SIZE)
                .color(TFT_GREEN)                             // Setting the color for the line
                .backgroud(TFT_WHITE)                         // Setting the color for the background
                .draw(&tft);
            //delay(190);
        }
    }
    delay(50);
}

void displayCombinedWaveform() {
    radar.Breath_Heart();

    if (radar.sensor_report == HEARTRATEWAVE) {
        unsigned int heart_wave_vals[] = {
            radar.heart_point_1, radar.heart_point_2, radar.heart_point_3,
            radar.heart_point_4, radar.heart_point_5
        };

        for (int i = 0; i < 5; ++i) {
            if (heart_data.size() > MAX_SIZE) {
                heart_data.pop();
            }
            heart_data.push(heart_wave_vals[i]);

            // Settings for the heart rate graph title
            auto heart_header = text(0, 0)
                                  .value("Heart Rate Waveform")
                                  .align(center)
                                  .valign(vcenter)
                                  .width(tft.width())
                                  .thickness(2);

            heart_header.height(heart_header.font_height(&tft) * 1.5);
            heart_header.draw(&tft);

            // Settings for the heart rate line graph
            auto heart_content = line_chart(20, heart_header.height());
            heart_content
                .height((tft.height() / 2) - heart_header.height() - 10)  // half the height of the screen minus the header
                .width(tft.width() - heart_content.x() * 2)
                .based_on(0.0)
                .show_circle(true)
                .max_size(MAX_SIZE)
                .backgroud(TFT_WHITE);

            // Passing heart data with blue color
            heart_content.value(heart_data).color(TFT_BLUE).draw(&tft);

            //delay(190);
        }
    }

    if (radar.sensor_report == BREATHWAVE) {
        unsigned int breath_wave_vals[] = {
            radar.breath_point_1, radar.breath_point_2, radar.breath_point_3,
            radar.breath_point_4, radar.breath_point_5
        };

        for (int i = 0; i < 5; ++i) {
            if (breath_data.size() > MAX_SIZE) {
                breath_data.pop();
            }
            breath_data.push(breath_wave_vals[i]);

            // Settings for the breath rate graph title
            auto breath_header = text(0, (tft.height() / 2) + 10)
                                  .value("Breath Waveform")
                                  .align(center)
                                  .valign(vcenter)
                                  .width(tft.width())
                                  .thickness(2);

            breath_header.height(breath_header.font_height(&tft) * 1.5);
            breath_header.draw(&tft);

            // Settings for the breath rate line graph
            auto breath_content = line_chart(20, breath_header.y() + breath_header.height());
            breath_content
                .height((tft.height() / 2) - breath_header.height() - 30)  // half the height of the screen minus the header
                .width(tft.width() - breath_content.x() * 2)
                .based_on(0.0)
                .show_circle(true)
                .max_size(MAX_SIZE)
                .backgroud(TFT_WHITE);

            // Passing breath data with green color
            breath_content.value(breath_data).color(TFT_GREEN).draw(&tft);

            //delay(190);
        }
    }

    delay(50);
}


void displaySleepInfo() {
    radar.SleepInf_Decode();
    bool currentInBed = radar.sensor_report == INBED;
    int currentAwakeTime = radar.awake_time;
    int currentLightTime = radar.light_time;
    int currentDeepTime = radar.deep_time;
    int currentSleepScore = radar.sleep_score;
    int currentTurnNum = radar.turn_num;
    int currentApneaNum = radar.apnea_num;
    int currentBreathRate = radar.breath_rate;
    int currentHeartRate = radar.heart_rate;

    if (previousInBed != currentInBed ||
        previousAwakeTime != currentAwakeTime ||
        previousLightTime != currentLightTime ||
        previousDeepTime != currentDeepTime ||
        previousSleepScore != currentSleepScore ||
        previousTurnNum != currentTurnNum ||
        previousApneaNum != currentApneaNum ||
        previousBreathRate != currentBreathRate ||
        previousHeartRate != currentHeartRate) {

        tft.fillScreen(TFT_WHITE); // Clear the screen before displaying new info

        tft.setCursor(10, 10);
        tft.setTextSize(3);
        tft.print("Sleep Info");

        tft.setTextSize(2);

        // Left half of the screen
        tft.setCursor(10, 60);
        tft.print("In Bed: ");
        tft.print(currentInBed ? "Yes" : "No");

        tft.setCursor(10, 90);
        tft.print("Awake: ");
        tft.print(currentAwakeTime);
        tft.print(" min");

        tft.setCursor(10, 120);
        tft.print("Deep: ");
        tft.print(currentDeepTime);
        tft.print(" min");

        tft.setCursor(10, 150);
        tft.print("Light: ");
        tft.print(currentLightTime);
        tft.print(" min");

        tft.setCursor(10, 180);
        tft.print("Sleep Score: ");
        tft.print(currentSleepScore);

        // Right half of the screen
        tft.setCursor(180, 60);
        tft.print("Turns: ");
        tft.print(currentTurnNum);

        tft.setCursor(180, 90);
        tft.print("Apnea: ");
        tft.print(currentApneaNum);

        tft.setCursor(180, 120);
        tft.print("Breath: ");
        tft.print(currentBreathRate);

        tft.setCursor(180, 150);
        tft.print("Heart: ");
        tft.print(currentHeartRate);

        // Update previous values
        previousInBed = currentInBed;
        previousAwakeTime = currentAwakeTime;
        previousLightTime = currentLightTime;
        previousDeepTime = currentDeepTime;
        previousSleepScore = currentSleepScore;
        previousTurnNum = currentTurnNum;
        previousApneaNum = currentApneaNum;
        previousBreathRate = currentBreathRate;
        previousHeartRate = currentHeartRate;
    }

    delay(200);
}


void displayMenu() {
    const char* modes[] = {
        "Heart and Breath Rates", "Human Existence", "Bodysign Graph",
        "Heart Rate Waveform", "Breath Waveform", "Combined Waveform", "Sleep Info"
    };
    int num_modes = sizeof(modes) / sizeof(modes[0]);
    int selected_mode = 0;
    int previous_selected_mode = -1;

    while (true) {
        if (selected_mode != previous_selected_mode) {
            tft.fillScreen(TFT_WHITE);
            tft.setTextColor(TFT_BLACK);
            tft.setTextSize(2);
            
            for (int i = 0; i < num_modes; ++i) {
                tft.setCursor(20, 30 + i * 30);
                if (i == selected_mode) {
                    tft.setTextColor(TFT_RED);
                    tft.print("> ");
                } else {
                    tft.print("  ");
                }
                tft.print(modes[i]);
                tft.setTextColor(TFT_BLACK);
            }
            previous_selected_mode = selected_mode;
        }

        if (digitalRead(WIO_5S_UP) == LOW) {
            selected_mode = (selected_mode - 1 + num_modes) % num_modes;
            delay(200); // Debounce delay
        }
        if (digitalRead(WIO_5S_DOWN) == LOW) {
            selected_mode = (selected_mode + 1) % num_modes;
            delay(200); // Debounce delay
        }
        if (digitalRead(WIO_5S_PRESS) == LOW) {
            display_mode = selected_mode;
            //radar.reset_func();
            tft.fillScreen(TFT_WHITE);
            tft.setTextColor(TFT_BLACK);
            resetValues();
            delay(200); // Debounce delay
            inMenu = false;
            break;
        }
        if (digitalRead(WIO_KEY_A) == LOW) {
            inMenu = false;
            display_mode = lastDisplayModeBeforeMenu;  // Return to the last display mode
            //radar.reset_func();
            tft.fillScreen(TFT_WHITE);
            tft.setTextColor(TFT_BLACK);
            resetValues();
            delay(200); // Debounce delay
            break;
        }
    }
}

void loop() {
    static int last_display_mode = -1;  // Keeps track of the last display mode

    // Check button press to enter menu
    if (digitalRead(WIO_KEY_A) == LOW) {
        if (!inMenu) {
            lastDisplayModeBeforeMenu = display_mode;  // Save the current display mode
            inMenu = true;
            displayMenu();
        }
    }

    if (digitalRead(WIO_5S_PRESS) == LOW) {
        display_mode = (display_mode + 1) % 7;  // Cycle through 7 display modes
        //radar.reset_func();
        tft.fillScreen(TFT_WHITE);
        tft.setTextColor(TFT_BLACK);
        resetValues();
        delay(500);  // Debounce delay
    }

    // Only change radar mode if display mode has changed
    if (display_mode != last_display_mode) {
        if (display_mode == 6) {
            radar.reset_func();
            delay(50);
            radar.ModeSelect_fuc(2);  // Sleep state transfer mode
            Serial.println("Radar set to sleep state transfer mode.");
        } else {
            radar.ModeSelect_fuc(1);  // Real-time data transfer mode
            Serial.println("Radar set to real-time mode.");
        }
        last_display_mode = display_mode;
    }

    switch (display_mode) {
        case 0:
            displayHeartAndBreathRates();
            break;
        case 1:
            displayHumanExistence();
            break;
        case 2:
            displayBodysignGraph();
            break;
        case 3:
            displayHeartRateGraph();
            break;
        case 4:
            displayBreathWaveform();
            break;
        case 5:
            displayCombinedWaveform();
            break;
        case 6:
            displaySleepInfo();
            break;
    }
}