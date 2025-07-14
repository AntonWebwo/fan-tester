#include "PWM.hpp"
#include <LiquidCrystal_I2C.h>

#define BUTTON_PIN 4
#define PWM_PIN 3
#define SPEED_PIN 2 
#define LED_RED_PIN 5
#define LED_GREEN_PIN 6

PWM my_pwm(SPEED_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool testRunning = false;
unsigned long testStartTime = 0;
bool dataChanged = false;
int lastInputValue = 0;

void setup() {
    Serial.begin(115200);
    my_pwm.begin(true);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(PWM_PIN, OUTPUT);    
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    lcd.init();
    lcd.backlight();
    analogWrite(PWM_PIN, 0);
    lcd.clear();
    lcd.print("PRESS START TEST");    
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW && !testRunning) {
        lcd.clear();
        startTest();
    }

    if (testRunning) {
        unsigned long currentTime = millis();
        int inputValue = my_pwm.getValue();
        int pwmPercentage = calculatePWM(inputValue);
        
        if (inputValue != lastInputValue) {
            dataChanged = true;
            lastInputValue = inputValue;
        }

        lcd.setCursor(0, 0);
        lcd.print("PWM value ");
        lcd.print(inputValue);
        lcd.setCursor(0, 1);
        lcd.print("SPEED: ");
        lcd.print(pwmPercentage);
        lcd.print("%");

        if (currentTime - testStartTime >= 3000) {
            stopTest();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("TEST RESULT:");
            lcd.setCursor(0, 1);
            if (dataChanged) {
                lcd.print("FAN OK");
                digitalWrite(LED_GREEN_PIN, HIGH);
                digitalWrite(LED_RED_PIN, LOW);
            } else {
                lcd.print("FAN ERROR");
                digitalWrite(LED_RED_PIN, HIGH);
                digitalWrite(LED_GREEN_PIN, LOW);
            }
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("PRESS START TEST");  
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_GREEN_PIN, LOW);
        }
    }

    delay(100);
}

void startTest() {
    testRunning = true;
    testStartTime = millis();
    analogWrite(PWM_PIN, 255);
    dataChanged = false;
    lastInputValue = my_pwm.getValue();
}

void stopTest() {
    testRunning = false;
    analogWrite(PWM_PIN, 0);
}

int calculatePWM(int value) {
    if (value > 13000) {
        return 10;
    } else if (value > 8000) {
        return 20;
    } else if (value > 6000) {
        return 30;
    } else if (value > 5000) {
        return 40;
    } else if (value > 4500) {
        return 50;
    } else if (value > 4000) {
        return 60;
    } else if (value >= 3700 && value <= 4000) {
        return 70;
    } else if (value >= 3500 && value < 3700) {
        return 80;
    } else if (value >= 3300 && value < 3500) {
        return 90;
    } else if (value >= 3200 && value < 3300) {
        return 100;
    } else if (value > 3000) {
        return 0;
    } else {
        return 0;
    }
}
