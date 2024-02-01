/****************************************************************************/
//	Function: Main file for MP3 Player with esp32
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
#include <IRremote.hpp>
#include <DFRobotDFPlayerMini.h>

#define DEBUG false // set to true for debug output, false for no debug output
#define DEBUG_SERIAL \
  if (DEBUG)         \
  Serial

//MP3 mp3(MP3_RX, MP3_TX);
DFRobotDFPlayerMini mp3;
HardwareSerial mySoftwareSerial(1);

bool firstPlay = true;
bool play = false;     // Play or pause
bool muteOn = false;
unsigned long lastExecutionTimeIR = 0;
int currentVolume = INITIAL_VOLUME;

void setup()
{
  DEBUG_SERIAL.begin(115200);
  DEBUG_SERIAL.println("setup(): begin");
  delay(500);
  mySoftwareSerial.begin(9600, SERIAL_8N1, MP3_RX, MP3_TX); // speed, type, RX, TX

  if (!mp3.begin(mySoftwareSerial))
  { // Use softwareSerial to communicate with mp3.

    DEBUG_SERIAL.println("Unable to begin:");
    DEBUG_SERIAL.println("1.Please recheck the connection!");
    DEBUG_SERIAL.println("2.Please insert the SD card!");
    while (true)
      ;
  }
  DEBUG_SERIAL.println(F("DFPlayer Mini online."));

  // Button config
  pinMode(BUTTON_PIN, INPUT);
  buttonConfig.setEventHandler(handleEvent);
  buttonConfig.setFeature(ButtonConfig::kFeatureClick);
  buttonConfig.setClickDelay(500);
  
  // IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  playWelcomeTune();

  DEBUG_SERIAL.println("setup(): end");
}

void loop()
{
  checkButtons();
  receiveIRData();
  checkStatus();
}

void playWelcomeTune()
{
  delay(500);
  mp3.volume(20);
  mp3.play(1);
  delay(5000);
  mp3.volume(currentVolume);
}

void checkStatus()
{
  static uint32_t prevMillis = millis();
  uint32_t currentMillis = millis();

  if (currentMillis - prevMillis >= 1000)
  {
    int playerState = mp3.readState();
    DEBUG_SERIAL.print("Player state: ");
    DEBUG_SERIAL.println(playerState);

    // Player state 0 or 2 means not playing
    if (playerState == 0 || playerState == 2)
    {
      // Check if it has been in this state for more than 1 minute (60000)
      if (currentMillis - prevMillis >= 60000)
      {
        // Put ESP32 into deep sleep for 1 minute
        //esp_sleep_enable_timer_wakeup(60 * 1000000); // 1 minute in microseconds
        DEBUG_SERIAL.println("Going to sleep");
        mp3.sleep();
        esp_deep_sleep_start();
      }
    }
    prevMillis = currentMillis;
  }
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
      DEBUG_SERIAL.println("Previous song button pressed");
      mp3.previous();
      break;
    case 2:
      DEBUG_SERIAL.println("Play / pause pressed");
      playOrPause();
      break;
    case 3:
      DEBUG_SERIAL.println("Next song button pressed");
      mp3.next();
      break;
    }
  }
}

void playOrPause()
{
  play = !play;
  if (play)
  {
    DEBUG_SERIAL.print("Firstplay: ");
    DEBUG_SERIAL.println(firstPlay);
    if (firstPlay == true)
    {
      mp3.loopFolder(2);
      firstPlay = false;
    }
    else
      mp3.start();
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
    mp3.volume(0);
  else
    mp3.volume(currentVolume);
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
    DEBUG_SERIAL.println("Executed command: IR_CMD_MUTE");
    muteOrUnmute();
    break;

  case IR_CMD_PREV_SONG:
    DEBUG_SERIAL.println("Executed command: IR_CMD_PREV_SONG");
    mp3.previous();
    break;

  case IR_CMD_NEXT_SONG:
    DEBUG_SERIAL.println("Executed command: IR_CMD_NEXT_SONG");
    mp3.next();
    break;

  case IR_CMD_VOL_DOWN:
    DEBUG_SERIAL.println("Executed command: IR_CMD_VOL_DOWN");
    currentVolume = max(0, currentVolume - 1);
    mp3.volume(currentVolume);
    break;

  case IR_CMD_VOL_UP:
    DEBUG_SERIAL.println("Executed command: IR_CMD_VOL_UP");
    currentVolume = min(MAX_VOLUME, currentVolume + 1);
    mp3.volume(currentVolume);
    break;

  case IR_CMD_PLAY_PAUSE:
    DEBUG_SERIAL.println("Executed command: IR_CMD_PLAY_PAUSE");
    playOrPause();
    break;
  }
}
