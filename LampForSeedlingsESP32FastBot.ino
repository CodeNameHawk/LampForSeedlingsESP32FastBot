/*
  LampForSeedlingsESP32FastBot
  https://github.com/CodeNameHawk/LampForSeedlingsESP32FastBot

  used with ESP32 board and FastBot library https://github.com/GyverLibs/FastBot
  This program is used to control a lamp for seedlings.
  The lamp can be turned on and off by a timer or by a Telegram bot.
*/

// #define DEBUG

String programName = "LampForSeedlingsESP32FastBot";
String programVersion = "v.1 2025.03.13";
String libraryVersion = "ESP32 board v.3.1.3, FastBot v.2.27.3";

// programVersion = "v.1 2025.03.13"

#include "Variables.ino" // Include global variables

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\nStarting " + programName + " " + programVersion);
  Serial.println(libraryVersion);
#ifdef DEBUG
  Serial.println("In Setup: Lamp is turned off.");
#endif
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Lampa, OUTPUT);
  digitalWrite(Lampa, HIGH); // Turn off the lamp
  shouldTurnOnLampByTimer = false;
  shouldTurnOnLampByTelegram = false;
  isLampOn = false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(500);
  while (WiFi.status() != WL_CONNECTED)
  {
#ifdef DEBUG
    Serial.print('.');
#endif
    delay(500);
  }

  // Sync time with NTP
  configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");

  // Set the Telegram bot properies
  myBot.setPeriod(2000);

  // Add the callback function to bot
  myBot.attach(newMsg);

  // Check if all things are ok
  messageText = "-----      " + programName + " " + programVersion + " " + libraryVersion + "    -----" +
                "\n\n List of commands.\n /Light_on ||  /Light_off  ||  /Status  ||  /Timer ";
  // Send a message to specific user who has started your bot
  messageText += "\n FreeHeap = ";
  messageText += String(ESP.getFreeHeap());

#ifdef DEBUG
  Serial.print("\nTest Telegram connection... ");
  myBot.sendMessage(messageText, userid) ? Serial.println("OK") : Serial.println("NOK");
#else
  myBot.sendMessage(messageText, userid);
#endif

  // Initialize LittleFS and send the result to the user
  myBot.sendMessage(initLittleFS(), userid);

  initOTA();
  controlLamp();
}

void loop()
{
  static unsigned long prevWorkTime = millis();

  myBot.tick();

  ArduinoOTA.handle();

  if (millis() - prevWorkTime > 1000)
  {
    prevWorkTime = millis();
    time(&currentTime);
    localtime_r(&currentTime, &timeinfo);
    uint16_t CurrentTimeInMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;

    // Below check if lamp needs to be turned on by Timer, defined in timerintervals.txt
    shouldTurnOnLampByTimer = false;

    // Check all timer intervals
    for (uint16_t i = 0; i < timerTimesCount; i++)
    {
      // Stop checking if we hit an empty interval (StartTime == 0 && EndTime == 0)
      if (TimerTimes[i].StartTime == 0 && TimerTimes[i].EndTime == 0)
      {
        break;
      }
      // Check if current time falls within this interval
      if (CurrentTimeInMinutes >= TimerTimes[i].StartTime && CurrentTimeInMinutes < TimerTimes[i].EndTime)
      {
        shouldTurnOnLampByTimer = true;
        break;
      } // end if (CurrentTimeInMinutes >= TimerTimes[i].StartTime && CurrentTimeInMinutes < TimerTimes[i].EndTime)
    } // end for (uint16_t i = 0; i < timerTimesCount; i++)
    // Control the light based on timing or telegram command
    controlLamp();
  }
}
