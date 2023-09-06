#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#define PIN_BUTTON1 2
#define PIN_BUTTON2 3
#define PIN_BUTTON3 4
#define PIN_BUZZER 12

// lcd connections
#define LCD_RS 5
#define LCD_RW 6
#define LCD_EN 7
#define LCD_D4 8
#define LCD_D5 9
#define LCD_D6 10
#define LCD_D7 11

#define BTN_PAD_VALUE_INC_TIME 4
#define BTN_PAD_VALUE_DEC_TIME 2
#define BTN_PAD_VALUE_ABRT     6
#define BTN_PAD_VALUE_SP       1

// * Signals of the Key pad
typedef enum{
    NO_PRESSED,
    BOUNCE,
    PRESSED
}button_state_t;

typedef enum{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION
}event_status_t;

// * Signals of the App
typedef enum{
    INC_TIME,
    DEC_TIME,
    TIME_TICK,
    START_PAUSE,
    ABRT,
    // * Internal Signals
    ENTRY,
    EXIT
}protimer_signal_t;

struct protime_tag;
struct event_tag;

typedef event_status_t (*protimer_state_t)(struct protime_tag *const, struct event_tag const *const);

// * Main app structure
typedef struct protime_tag{
    uint32_t curr_time;
    uint32_t elapsed_time;
    uint32_t pro_time;
    protimer_state_t active_state;
}protimer_t;

// * Super event structure
typedef struct event_tag{
    uint8_t sig;
}event_t;

// * User generated signals
typedef struct{
    event_t super;
}protimer_user_event_t;


// * Tick event
typedef struct{
    event_t super;
    uint8_t ss;
}protimer_tick_event_t;

void protimer_init(protimer_t *mobj);
event_status_t protimer_state_machine(protimer_t *const mobj, event_t const *const e);
#endif