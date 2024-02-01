#define MP3_RX 16 // should connect to TX of the Serial MP3 Player module
#define MP3_TX 17 // connect to RX of the module

#define IR_RECEIVE_PIN 18 // Connected GPIO Pin for the IR Receiver

// defines for the IR Remote decoded Commands
#define IR_CMD_MUTE       71
#define IR_CMD_PREV_SONG  64
#define IR_CMD_NEXT_SONG  67
#define IR_CMD_VOL_DOWN   21
#define IR_CMD_VOL_UP     9
#define IR_CMD_PLAY_PAUSE 68

#define IR_EXECUTION_TIME_WINDOW 100
#define BUTTON_EXECUTION_TIME_WINDOW 10

#define CYCLE_PLAY_OFF 1
#define CYCLE_PLAY_ON  0

#define INITIAL_VOLUME 15 // equals 10. Volume can be from 0 to 30
#define MAX_VOLUME     22 // equals 20