/****************************************************************************/
//	Function: Main file for MP3 Player with esp32
//	Arduino IDE: Arduino-1.6.5
//	Author:	 SirTheta
//	Date: 	 03.01.2024
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
/****************************************************************************/

#include <main.h>
#include <defines.h>
#include <Arduino.h>
#include <RedMP3.h>
#include <IRremote.hpp>

MP3 mp3(MP3_RX, MP3_TX);

unsigned long playmode = CYCLE_PLAY_OFF; // 0 is all songs cycle play, 1 is single cycle play
unsigned long play = 0;     // Play or pause
unsigned long muteOn = 0;
unsigned long lastExecutionTimeIR = 0;
unsigned long lastExecutionTimeButton = 0;
int currentVolume = INITIAL_VOLUME;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("setup(): begin"));
  delay(500);
  mp3.begin();

  mp3.setVolume(currentVolume);
  // Button config
  pinMode(BUTTON_PIN, INPUT);
  buttonConfig.setEventHandler(handleEvent);
  buttonConfig.setFeature(ButtonConfig::kFeatureClick);
  buttonConfig.setClickDelay(500);
  
  // IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.println(F("setup(): end"));
}

void loop()
{
  checkButtons();
  receiveIRData();
}

void checkButtons()
{
  static uint16_t prev = millis();
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
      mp3.previousSong();
      break;
    case 2:
      playOrPause();
      break;
    case 3:
      mp3.nextSong();
      break;
    }
  }
}

void playOrPause()
{
  play= !play;
  if (play)
  {    
    if (playmode == CYCLE_PLAY_OFF)
    {
      playmode = CYCLE_PLAY_ON;
      mp3.setCyleMode(playmode);
    }
    mp3.play();
  }
  else
    mp3.pause();
}

// function is executed when mute is pressed. if mute is pressed again
// the volume is restored to the previous value
void muteOrUnmute()
{
  muteOn = !muteOn;
  if (muteOn)
    mp3.setVolume(0);
  else
    mp3.setVolume(currentVolume);
}

// receive the infrared data
void receiveIRData()
{
  if (IrReceiver.decode())
  {
    // check execution window. If outside the window, the command is executed
    if ((millis() - lastExecutionTimeIR) >= IR_EXECUTION_TIME_WINDOW)
    {
      executeAction(IrReceiver.decodedIRData.command);
      lastExecutionTimeIR = millis();
    }

    IrReceiver.resume();
  }
}

void executeAction(int command)
{
  switch (command)
  {
  case IR_CMD_MUTE:
    Serial.println("Executed command: IR_CMD_MUTE");
    muteOrUnmute();
    break;

  case IR_CMD_PREV_SONG:
    Serial.println("Executed command: IR_CMD_PREV_SONG");
    mp3.previousSong();
    break;

  case IR_CMD_NEXT_SONG:
    Serial.println("Executed command: IR_CMD_NEXT_SONG");
    mp3.nextSong();
    break;

  case IR_CMD_VOL_DOWN:
    Serial.println("Executed command: IR_CMD_VOL_DOWN");
    currentVolume = max(0, currentVolume - 1);
    mp3.setVolume(currentVolume);
    break;

  case IR_CMD_VOL_UP:
    Serial.println("Executed command: IR_CMD_VOL_UP");
    currentVolume = min(MAX_VOLUME, currentVolume + 1); // Assuming max volume is 0x0F
    mp3.setVolume(currentVolume);
    break;

  case IR_CMD_PLAY_PAUSE:
    playOrPause();
    break;
  }
}
