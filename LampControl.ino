int timeStringToMinutes(String timeStr)
{
  int hours = timeStr.substring(0, 2).toInt();
  int minutes = timeStr.substring(3, 5).toInt();
  return hours * 60 + minutes;
}

bool loadTimerIntervalsFromFile()
{
  File file = LittleFS.open("/timerintervals.txt", "r");
  if (!file)
  {
#ifdef DEBUG
    Serial.println("Failed to open timerintervals.txt");
#endif
    return false;
  }

  int index = 0;
  while (file.available() && index < 1440)
  {
    String line = file.readStringUntil('\n');
    line.trim();

    // Skip empty lines and comments
    if (line.length() == 0 || line.startsWith("#"))
    {
      continue;
    }

    // Parse line format "HH:MM HH:MM"
    if (line.length() >= 11)
    {                                             // Minimum length for "HH:MM HH:MM"
      String startTimeStr = line.substring(0, 5); // Get "HH:MM"
      String endTimeStr = line.substring(6, 11);  // Get "HH:MM" after space

      TimerTimes[index].StartTime = timeStringToMinutes(startTimeStr);
      TimerTimes[index].EndTime = timeStringToMinutes(endTimeStr);
      index++;
    }
  }
  file.close();
#ifdef DEBUG
  Serial.printf("Loaded %d timer intervals\n", index);
#endif

  timerTimesCount = index; // remember the number of read intervals
  return true;
}

void controlLamp(void)
{
  if (shouldTurnOnLampByTimer || shouldTurnOnLampByTelegram)
  {
    // if lamp was not turned on before, turn it on
    if (isLampOn == false)
    {
      digitalWrite(Lampa, LOW); // Turn on the lamp
      isLampOn = true;  // Remember that it's on
      messageText = "ON - Lamp turned on ";
      if (shouldTurnOnLampByTimer && shouldTurnOnLampByTelegram)
      {
        messageText += "by TimerStart and by TelegramLampON\n";
      }
      else
      {
        messageText += shouldTurnOnLampByTimer ? "by TimerStart\n" : "";
        messageText += shouldTurnOnLampByTelegram ? "by TelegramLampON\n" : "";
      }

      // Add active timer interval information when lamp is turned on by timer
      if (shouldTurnOnLampByTimer)
      {
        for (uint16_t i = 0; i < timerTimesCount; i++)
        {
          uint16_t CurrentTimeInMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
          if (CurrentTimeInMinutes >= TimerTimes[i].StartTime && CurrentTimeInMinutes < TimerTimes[i].EndTime)
          {
            messageText += "Timer interval: ";
            messageText += String(TimerTimes[i].StartTime / 60) + ":";
            messageText += (TimerTimes[i].StartTime % 60 < 10 ? "0" : "") + String(TimerTimes[i].StartTime % 60);
            messageText += " - ";
            messageText += String(TimerTimes[i].EndTime / 60) + ":";
            messageText += (TimerTimes[i].EndTime % 60 < 10 ? "0" : "") + String(TimerTimes[i].EndTime % 60);
            messageText += "\n";
            break;
          }
        }
      }

      messageText += "Local Time: ";
      messageText += timeinfo.tm_hour < 10 ? "0" + String(timeinfo.tm_hour) : String(timeinfo.tm_hour);
      messageText += timeinfo.tm_min < 10 ? ":0" + String(timeinfo.tm_min) : ":" + String(timeinfo.tm_min);
      myBot.sendMessage(messageText, userid);
#ifdef DEBUG
      Serial.println(messageText);
#endif
    }
  }
  else
  {
    if (isLampOn == true)
    {
      digitalWrite(Lampa, HIGH);
      isLampOn = false;
      messageText = "OFF - Lamp turned off\n";
      messageText += "Local Time: ";
      messageText += timeinfo.tm_hour < 10 ? "0" + String(timeinfo.tm_hour) : String(timeinfo.tm_hour);
      messageText += timeinfo.tm_min < 10 ? ":0" + String(timeinfo.tm_min) : ":" + String(timeinfo.tm_min);
      myBot.sendMessage(messageText, userid);
#ifdef DEBUG
      Serial.println(messageText);
#endif
    }
  }
}
