#include <Arduino.h>
#include "ButtonHandler.h"
#include "MQTTHandler.h"

const int relayPin = 14;
const int buttonPin = 4;
const int ledPin = 2;

bool relayState = false;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void IRAM_ATTR handleButtonPress()
{
  buttonPressed = true;
  Serial.println("Button pressed");
}

void setup_button()
{
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);
}

void handle_button()
{
  if (buttonPressed)
  {
    delay(50);
    bool currentButtonState = digitalRead(buttonPin);

    if (currentButtonState == HIGH)
    {
      buttonPressed = false;
      unsigned long currentTime = millis();
      if ((currentTime - lastDebounceTime) > debounceDelay)
      {
        relayState = !relayState;
        digitalWrite(relayPin, relayState ? HIGH : LOW);
        digitalWrite(ledPin, relayState ? LOW : HIGH);
        lastDebounceTime = currentTime;

        client.publish(topic_switch.c_str(), relayState ? "on" : "off");
      }
    }
  }
}

void update_switch(String state)
{
  digitalWrite(relayPin, state == "on" ? HIGH : LOW);
  digitalWrite(ledPin, state == "on" ? LOW : HIGH);
}
