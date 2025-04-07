#include <Arduino.h>
#include "ButtonHandler.h"
#include "MQTTHandler.h"

const int relayPin = 14;
const int buttonPin = 4;
const int ledPin = 2;

bool relayState = true;
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

  client.publish(topic_switch.c_str(), String(relayState).c_str(), true);
}

void handle_button()
{
  if (buttonPressed)
  {
    delay(10);
    bool currentButtonState = digitalRead(buttonPin);

    if (currentButtonState == HIGH)
    {
      buttonPressed = false;
      unsigned long currentTime = millis();
      if ((currentTime - lastDebounceTime) > debounceDelay)
      {
        set_switch(relayState);
        lastDebounceTime = currentTime;
      }
    }
  }
}

void update_switch(String state)
{
  relayState = state == "0" ? HIGH : LOW;
  digitalWrite(relayPin, relayState);
  digitalWrite(ledPin, relayState);
}

void set_switch(bool state)
{
  relayState = state;
  digitalWrite(relayPin, !relayState);
  digitalWrite(ledPin, !relayState);
  client.publish(topic_switch.c_str(), String(relayState).c_str(), true);
}