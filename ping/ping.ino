#include <avr/wdt.h>

#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

boolean ON = LOW;
boolean OFF = HIGH;

boolean RED[] = {ON, OFF, OFF};
boolean GREEN[] = {OFF, ON, OFF};
boolean BLUE[] = {OFF, OFF, ON};
boolean YELLOW[] = {ON, ON, OFF};
boolean CYAN[] = {OFF, ON, ON};
boolean MAGENTA[] = {ON, OFF, ON};
boolean WHITE[] = {ON, ON, ON};
boolean BLACK[] = {OFF, OFF, OFF};

// {R, G, B}
int DIGITAL_OUTS[] = {RED_PIN, GREEN_PIN, BLUE_PIN};

char inByte = '\0';
unsigned long lastRead = 0;
unsigned long lastBlink = 0;
const long blinkInterval = 500;
boolean isBlinking = false;
boolean currentBlinkColor = false; // false for blinkColor1, true for blinkColor2
boolean* blinkColor1;
boolean* blinkColor2;

void setColor(boolean* color) {
    for (int i = 0; i < 3; i++) {
        digitalWrite(DIGITAL_OUTS[i], color[i]);
    }
}

void startBlink(boolean* color1, boolean* color2 = BLACK) {
    isBlinking = true;
    blinkColor1 = color1;
    blinkColor2 = color2;
    lastBlink = millis();
}

void updateBlink() {
    if (isBlinking && millis() - lastBlink >= blinkInterval) {
        lastBlink = millis();
        setColor(currentBlinkColor ? blinkColor1 : blinkColor2);
        currentBlinkColor = !currentBlinkColor; // toggle the current blink color
    }
}

void setup() {    
    Serial.begin(115200);
    for (int i = 0; i < 3; i++) {
        pinMode(DIGITAL_OUTS[i], OUTPUT);
    }
    setColor(MAGENTA);
    delay(1000);
    setColor(BLACK);
    wdt_enable(WDTO_1S);
}

void loop() {
    wdt_reset();

    if (Serial.available()) { // new message
        isBlinking = false;
        inByte = Serial.read();
        lastRead = millis();

        switch (inByte) {
            case '1': // good
                setColor(GREEN);
                break;
            case '2': // med
                setColor(YELLOW);
                break;
            case '3': // bad
                setColor(RED);
                break;
            case '4': // no connection
                startBlink(RED, BLUE);
                break;
            case '5': // intermittent (medium latency)
                startBlink(YELLOW, BLUE);
                break;
            case '6': // intermittent (good latency)
                startBlink(GREEN, BLUE);
                break;
            case '7': // intermittent (bad latency)
                startBlink(RED);
                break;
            default: // off
                setColor(BLACK);
        }
    } else if (millis() - lastRead >= 10000) {
        setColor(BLACK);
    }

    updateBlink();
}

