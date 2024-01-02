#include <main.h>
#include <defines.h>
#include <Arduino.h>
#include <RedMP3.h>

MP3 mp3(MP3_RX, MP3_TX);

unsigned char playmode = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("setup(): begin"));
  delay(500);
  mp3.begin();

  pinMode(BUTTON_PIN, INPUT);
  buttonConfig.setEventHandler(handleEvent);
  buttonConfig.setFeature(ButtonConfig::kFeatureClick);
  buttonConfig.setClickDelay(500);

  Serial.println(F("setup(): end"));
}

void loop()
{
  checkButtons();
}

void checkButtons()
{
  static uint16_t prev = millis();

  // DO NOT USE delay(5) to do this.
  // The (uint16_t) cast is required on 32-bit processors, harmless on 8-bit.
  uint16_t now = millis();
  if ((uint16_t)(now - prev) >= 5)
  {
    prev = now;
    buttonConfig.checkButtons();
  }
}

// The event handler for the buttons.
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
  int val = analogRead(BUTTON_PIN);
  if (eventType == AceButton::kEventClicked)
  {
    switch (button->getPin())
    {
    case 1:
      //Serial.println("Button K1 was pressed");
      mp3.previousSong();
      break;
    case 2:
      //Serial.println("Button K2 was pressed");
      playOrPause();
      break;
    case 3:
      //Serial.println("Button K3 was pressed");
      mp3.nextSong();
      break;
    }
  }
}

void playOrPause()
{
  playmode = !playmode;
  if (playmode == PLAY)
    mp3.play();
  else
    mp3.pause();
}
