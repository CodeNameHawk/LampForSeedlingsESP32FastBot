// Message handler
void newMsg(FB_msg &msg)
{
    String msgText = msg.text;
#ifdef DEBUG
    // output all information about the message
    Serial.println(msg.toString());
#endif

    if (msg.isFile)
    {
        handleIsFileCommand(msg);
    }
    else if (msgText.equalsIgnoreCase("/Timer"))
    {
        handleTimerCommand();
    }
    else if (msgText.equalsIgnoreCase("/Status"))
    {
        handleStatusCommand();
    }
    else if (msgText.equals("/Light_on"))
    {
        handleLightOnCommand();
    }
    else if (msgText.equals("/Light_off"))
    {
        handleLightOffCommand();
    }
}

void handleIsFileCommand(FB_msg &msg)
{
#ifdef DEBUG
    Serial.print("Downloading ");
    Serial.println(msg.fileName);
#endif

    String path = '/' + msg.fileName;
    File f = LittleFS.open(path, "w");
    if (!f)
    {
#ifdef DEBUG
        Serial.println("Failed to open file for writing");
#endif
        myBot.sendMessage("Failed to create file", userid);
        return;
    }

    bool status = myBot.downloadFile(f, msg.fileUrl);
    f.close();

    if (status)
    {
        // Verify file was written
        File f = LittleFS.open(path, "r");
        if (!f || f.size() == 0)
        {
#ifdef DEBUG
            Serial.println("File verification failed");
#endif
            myBot.sendMessage("File write verification failed", userid);
        }
        else
        {
#ifdef DEBUG
            Serial.println("File downloaded and verified successfully");
#endif

            // If this is the timer intervals file, reload it
            if (msg.fileName == "timerintervals.txt")
            {
                if (loadTimerIntervalsFromFile())
                {
                    myBot.sendMessage("File saved and timer intervals reloaded", userid);
                }
                else
                {
                    myBot.sendMessage("File saved but failed to reload timer intervals", userid);
                }
            }
            else
            {
                myBot.sendMessage("File saved successfully", userid);
            }
        }
        if (f)
            f.close();
    }
    else
    {
#ifdef DEBUG
        Serial.println("File download failed");
#endif
        myBot.sendMessage("File download failed", userid);
        // Clean up failed file
        LittleFS.remove(path);
    }
}

void handleStatusCommand()
{
#ifdef DEBUG
    Serial.println("\r\nShow Status");
#endif
    if (isLampOn == true)
    {
        messageText = "ON - Lamp is turned on";
    }
    else
    {
        messageText = "OFF - Lamp is turned off";
    }
    if (shouldTurnOnLampByTimer && shouldTurnOnLampByTelegram)
    {
        messageText += " by TimerStart and TelegramLampON\n";
    }
    else
    {
        messageText += shouldTurnOnLampByTimer ? " by TimerStart" : "";
        messageText += shouldTurnOnLampByTelegram ? " by TelegramLampON" : "";
    }

    // Add active timer interval information
    if (shouldTurnOnLampByTimer)
    {
        for (uint16_t i = 0; i < timerTimesCount; i++)
        {
            uint16_t CurrentTimeInMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
            if (CurrentTimeInMinutes >= TimerTimes[i].StartTime && CurrentTimeInMinutes < TimerTimes[i].EndTime)
            {
                messageText += "\nActive Timer: ";
                messageText += String(TimerTimes[i].StartTime / 60) + ":";
                messageText += (TimerTimes[i].StartTime % 60 < 10 ? "0" : "") + String(TimerTimes[i].StartTime % 60);
                messageText += " - ";
                messageText += String(TimerTimes[i].EndTime / 60) + ":";
                messageText += (TimerTimes[i].EndTime % 60 < 10 ? "0" : "") + String(TimerTimes[i].EndTime % 60);
                break;
            }
        }
    }

    messageText += "\nFreeHeap  = ";
    messageText += String(ESP.getFreeHeap());
    messageText += "\nLocalTime = ";
    time(&currentTime);
    localtime_r(&currentTime, &timeinfo);
    messageText += timeinfo.tm_hour < 10 ? "0" + String(timeinfo.tm_hour) : String(timeinfo.tm_hour);
    messageText += timeinfo.tm_min < 10 ? ":0" + String(timeinfo.tm_min) : ":" + String(timeinfo.tm_min);
    messageText += "\n\n List of commands.\n /Light_on ||  /Light_off  ||  /Status  ||  /Timer ";

    myBot.sendMessage(messageText, userid); // send it
}

String initLittleFS()
{
    messageText = ""; // Clear the message text
    // Initialize LittleFS with proper error handling
    if (!LittleFS.begin(true))
    { // true parameter will format if mounting fails
#ifdef DEBUG
        Serial.println("LittleFS Mount Failed");
#endif
        messageText += "LittleFS Mount Failed\n";
        if (!LittleFS.format())
        {
#ifdef DEBUG
            Serial.println("LittleFS Format Failed");
#endif
            messageText += "LittleFS Format Failed\n";
        }
        if (!LittleFS.begin())
        {
#ifdef DEBUG
            Serial.println("LittleFS Mount Failed after formatting");
#endif
            messageText += "LittleFS Mount Failed after formatting\n";
        }
    }

    if (!loadTimerIntervalsFromFile())
    {
#ifdef DEBUG
        Serial.println("Failed to load timer intervals, using defaults");
#endif
        messageText += "Failed to load timer intervals, using defaults\n";
    }
    if (messageText.length() > 0)
    {
        return messageText;
    }
    else
    {
        return "LittleFS OK and Timer Intervals loaded";
    }
}

void handleTimerCommand()
{
    messageText = "Timer Intervals:\n";
    bool hasIntervals = false;

    for (int i = 0; i < timerTimesCount; i++)
    {
        if (TimerTimes[i].StartTime == 0 && TimerTimes[i].EndTime == 0)
        {
            break;
        }

        hasIntervals = true;
        int startHour = TimerTimes[i].StartTime / 60;
        int startMin = TimerTimes[i].StartTime % 60;
        int endHour = TimerTimes[i].EndTime / 60;
        int endMin = TimerTimes[i].EndTime % 60;

        messageText += String(i + 1) + ". ";
        messageText += (startHour < 10 ? "0" : "") + String(startHour) + ":";
        messageText += (startMin < 10 ? "0" : "") + String(startMin) + " - ";
        messageText += (endHour < 10 ? "0" : "") + String(endHour) + ":";
        messageText += (endMin < 10 ? "0" : "") + String(endMin) + "\n";
    }

    if (!hasIntervals)
    {
        messageText += "No timer intervals set";
    }

    myBot.sendMessage(messageText, userid);
}

void handleLightOnCommand()
{
    if (shouldTurnOnLampByTelegram == true && shouldTurnOnLampByTimer == true && isLampOn == true)
    {
        messageText = "Received TelegramLampON command";
        messageText += "\nLamp is already on because TimerStart and TelegramLampON are active";
        messageText += "\nLamp will continue to stay on after TimerStop command";
        myBot.sendMessage(messageText, userid);
        handleStatusCommand();
    }
    else if (shouldTurnOnLampByTelegram == false && shouldTurnOnLampByTimer == true && isLampOn == true)
    {
        shouldTurnOnLampByTelegram = true;
        messageText = "Received TelegramLampON command";
        messageText += "\nLamp is already on because TimerStart is active";
        messageText += "\nLamp will continue to stay on after TimerStop command";
        myBot.sendMessage(messageText, userid);
        handleStatusCommand();
    }
    else if (shouldTurnOnLampByTelegram == true && shouldTurnOnLampByTimer == false && isLampOn == true)
    {
        messageText = "Received TelegramLampON command";
        messageText += "\nLamp is already on because TelegramLampON is active";
        myBot.sendMessage(messageText, userid);
        handleStatusCommand();
    }
    else if (shouldTurnOnLampByTelegram == false && shouldTurnOnLampByTimer == false && isLampOn == false)
    {
        shouldTurnOnLampByTelegram = true;
        messageText = "Received TelegramLampON command\n";
        controlLamp();
    }
}

void handleLightOffCommand()
{
    if (shouldTurnOnLampByTelegram == false && isLampOn == false && shouldTurnOnLampByTimer == false)
    {
        messageText = "Received TelegramLampOFF command";
        messageText += "\nLamp is already off";
        myBot.sendMessage(messageText, userid);
        handleStatusCommand();
    }
    else if (shouldTurnOnLampByTelegram == true && isLampOn == true && shouldTurnOnLampByTimer == false)
    {
        shouldTurnOnLampByTelegram = false;
        messageText = "Received TelegramLampOFF command";
        controlLamp();
    }
    else if (shouldTurnOnLampByTelegram == false && isLampOn == true && shouldTurnOnLampByTimer == true)
    {
        messageText = "Received TelegramLampOFF command";
        messageText += "\nLamp will not be turned off now because TimerStart is active";
        messageText += "\nLamp will turn off when TimerStop occurs";
        myBot.sendMessage(messageText, userid);
        handleStatusCommand();
    }
    else if (shouldTurnOnLampByTelegram == true && isLampOn == true && shouldTurnOnLampByTimer == true)
    {
        shouldTurnOnLampByTelegram = false;
        messageText = "Received TelegramLampOFF command";
        messageText += "\nLamp will not be turned off now because TimerStart is active";
        messageText += "\nLamp will turn off when TimerStop occurs";
        myBot.sendMessage(messageText, userid);
        handleStatusCommand();
    }
}
