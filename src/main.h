#include <Arduino.h>
#include <AceButton.h>

using ace_button::AceButton;
using ace_button::ButtonConfig;
using ace_button::LadderButtonConfig;

//**************************************
// Configuration for the Buttons
static const uint8_t BUTTON_PIN = 34;


static const uint8_t NUM_BUTTONS = 4;
static AceButton b0(nullptr, 0);
static AceButton b1(nullptr, 1);
static AceButton b2(nullptr, 2);
static AceButton b3(nullptr, 3);
// button 4 cannot be used because it represents "no button pressed"
static AceButton *const BUTTONS[NUM_BUTTONS] = {
    &b0,
    &b1,
    &b2,
    &b3,
};

static const uint8_t NUM_LEVELS = NUM_BUTTONS + 1;
static const uint16_t LEVELS[NUM_LEVELS] = {
    0 /* 0%, short to ground */,
    700 /* 32%, 4.7 kohm */,
    1512 /* 50%, 10 kohm */,
    2320 /* 82%, 47 kohm */,
    2500,
};

static LadderButtonConfig buttonConfig(
    BUTTON_PIN, NUM_LEVELS, LEVELS, NUM_BUTTONS, BUTTONS);

void checkButtons();
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState);
void playOrPause();