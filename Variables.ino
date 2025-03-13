#ifndef VARIABLES_H
#define VARIABLES_H

#include <LittleFS.h>
#include <ArduinoOTA.h>
#include <FastBot.h>

// Timezone definition
#include <time.h>

// Timezone definition
// set time zone to Eastern European Time (EET) which is UTC+2
// define Daylight Saving Time rules
// EET is UTC + 2 hours
// Eastern European Summer Time (EEST) is UTC + 3 hours
// starts on last Sunday of March
// ends on last Sunday of October

#define MYTZ "EET-2EEST,M3.5.0/3,M10.5.0/4"

#include <TimeLib.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
WiFiClientSecure client;

extern void initOTA(void);
extern const char *ssid;
extern const char *pass;
extern const char *token;
extern const char *userid;

FastBot myBot(token);

bool shouldTurnOnLampByTimer = false;  // Records if the lamp needs to be turned off after Timer
bool shouldTurnOnLampByTelegram = false; // Set to true if ON command received from Telegram

bool isLampOn = false; // Records if the lamp is on

time_t currentTime;
struct tm timeinfo;

const uint8_t Lampa = 33;

String messageText;

struct TimerInterval
{
  uint16_t StartTime;
  uint16_t EndTime;
};

TimerInterval TimerTimes[720]; // Array for 24h * 60min = 1440 minutes / 2
uint16_t timerTimesCount = 0;   // Number of TIMER activations read from file

#endif // VARIABLES_H