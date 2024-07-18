#include "utils.h"
#include <Arduino.h>
#include "main.h"


void torchON() {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
}

void torchOFF() {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
}


