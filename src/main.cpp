#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Streaming.h>
#include "flipdot.h"
#include "flipdotUDP.h"

void sayHello();

uint8_t boards[] = {25, 25};
uint16_t framebuffer[50];

Flipdot flipdot(boards, 2);
FlipdotUDP flipdotUDP(&flipdot);

void setup() {
    flipdot.begin();
    flipdotUDP.begin();

    Serial.begin(115200);
    Serial << "https://github.com/fluepke/flipdot-platformio/" << endl;
    sayHello();

    WiFi.hostname("flipdot");
    WiFi.begin ("Geekz.Karibik", "frickeln");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
    	delay (500);
    	Serial << ".";
    }

    Serial << WiFi.localIP();

    sayHello();
}

void loop() {
    flipdotUDP.poll();
}

void sayHello() {
    for (uint8_t i=0; i<50; i++) {
        framebuffer[i] = 0b1010101010101010 >> (i % 2);
    }
    flipdot.renderFrame(framebuffer);

    delay(10);

    for (uint8_t i=0; i<50; i++) {
        framebuffer[i] = ~0;
    }
    flipdot.renderFrame(framebuffer);

    delay(10);
}
