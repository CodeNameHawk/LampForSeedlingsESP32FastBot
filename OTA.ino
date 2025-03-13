void initOTA(void)
{
    ArduinoOTA
        .onStart([]()
                 {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
        .onEnd([]()
               { Serial.println("\nEnd"); })
#ifdef DEBUG               
        .onProgress([](unsigned int progress, unsigned int total)
                    {
							  static unsigned int old_progress;
							  unsigned int now_progress;

							  now_progress = (progress / (total / 100));
							  if (now_progress == 0)
								  old_progress = 0;

							  if (now_progress >= (old_progress + 10))
							  {
								  Serial.printf("Progress: %3u%%\r\n", now_progress);
								  old_progress = now_progress;
							  } })
#endif                              
        .onError([](ota_error_t error)
                 {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

    ArduinoOTA.begin();
}
