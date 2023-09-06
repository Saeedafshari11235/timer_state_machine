
#include "main.h"
#include "lcd.h"

//prototype of helper functions 
static void display_time(uint32_t);
static void display_msg(String s, uint8_t c, uint8_t r);
static void display_clr(void);
static void do_beep(void);

//prototype of state handlers
event_status_t protimer_state_machine(protimer_t *const mobj, event_t const *const e);
static event_status_t protimer_state_handler_IDLE(protimer_t *const mobj,event_t const *const e);
static event_status_t protimer_state_handler_TIME_SET(protimer_t *const mobj,event_t const *const e);
static event_status_t protimer_state_handler_COUNTDOWN(protimer_t *const mobj,event_t const *const e);
static event_status_t protimer_state_handler_PAUSE(protimer_t *const mobj,event_t const *const e);
static event_status_t protimer_state_handler_STAT(protimer_t *const mobj,event_t const *const e);


#define IDLE        &protimer_state_handler_IDLE
#define TIME_SET    &protimer_state_handler_TIME_SET
#define COUNTDOWN   &protimer_state_handler_COUNTDOWN
#define PAUSE       &protimer_state_handler_PAUSE
#define STAT        &protimer_state_handler_STAT

void protimer_init(protimer_t *mobj){
    event_t ee;
    ee.sig = ENTRY;
    mobj->active_state = IDLE;
    mobj->pro_time = 0;   
    (*mobj->active_state)(mobj,&ee);
}

static event_status_t protimer_state_handler_IDLE(protimer_t *const mobj,event_t const *const e){
    switch (e->sig){
        case ENTRY:{
            mobj->curr_time = 0;
            mobj->elapsed_time = 0;
            display_time(0);
            display_msg("Set",0,0);
            display_msg("Time",0,1);
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clr();
            return EVENT_HANDLED;
        }
        case INC_TIME:{
            mobj->curr_time +=60;
            mobj->active_state = TIME_SET;
            return EVENT_TRANSITION;
        }
        case START_PAUSE:{
            mobj->active_state = STAT;
            return EVENT_TRANSITION;
        }
        case TIME_TICK:{
            if(((protimer_tick_event_t *)(e))->ss == 5){
                do_beep();
                return EVENT_HANDLED;
            }
        }
    }
    return EVENT_IGNORED;
}
static event_status_t protimer_state_handler_TIME_SET(protimer_t *const mobj,event_t const *const e){
    switch (e->sig){
        case ENTRY:{
            display_time((mobj->curr_time));
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clr();
            return EVENT_HANDLED;
        }
        case INC_TIME:{
            mobj->curr_time += 60;
            display_time(mobj->curr_time);
        }
        case DEC_TIME:{
            if (mobj->curr_time>=60){
                mobj->curr_time-=60;
                display_time(mobj->curr_time);
            }
        }
        case START_PAUSE:{
            if(mobj->curr_time>=60){
                mobj->active_state = COUNTDOWN;
                return EVENT_TRANSITION;
            }
        }
        case ABRT:{
            mobj->active_state = IDLE;
            return EVENT_TRANSITION;
        }
    }
    return EVENT_IGNORED;
}
static event_status_t protimer_state_handler_COUNTDOWN(protimer_t *const mobj,event_t const *const e){
    switch (e->sig){
        case EXIT:{
            mobj->pro_time += mobj->elapsed_time;
            mobj->elapsed_time = 0;
            return EVENT_HANDLED;
        }
        case TIME_TICK:{
            if (((protimer_tick_event_t *)(e))->ss == 10){
                --mobj->curr_time;
                ++mobj->elapsed_time;
                display_time((mobj->curr_time));
                if(!mobj->curr_time){
                    mobj->active_state = IDLE;
                    return EVENT_TRANSITION;
                }
                return EVENT_HANDLED;
            }
        }
        case ABRT:{
            mobj->active_state = IDLE;
            return EVENT_TRANSITION;
        }
        case START_PAUSE:{
            mobj->active_state=PAUSE;
            return EVENT_TRANSITION;
        }
    }
    return EVENT_IGNORED;
}
static event_status_t protimer_state_handler_PAUSE(protimer_t *const mobj,event_t const *const e){
    switch (e->sig){
        case ENTRY:{
            display_msg("PAUSED",5,1);
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clr();
            return EVENT_HANDLED;
        }
        case START_PAUSE:{
            mobj->active_state=COUNTDOWN;
            return EVENT_TRANSITION;
        }
        case INC_TIME:{
            mobj->curr_time+=60;
            mobj->active_state = TIME_SET;
            return EVENT_TRANSITION;
        }
        case ABRT:{
            mobj->active_state=IDLE;
            return EVENT_TRANSITION;
        }
        case DEC_TIME:{
            if(mobj->curr_time>=60){
                mobj->curr_time -=60;
                mobj->active_state = TIME_SET;
                return EVENT_TRANSITION;
            }
        }
    }
    return EVENT_IGNORED;
}
static event_status_t protimer_state_handler_STAT(protimer_t *const mobj,event_t const *const e){
    switch (e->sig){
        case ENTRY:{
            display_time((mobj->pro_time));
            display_msg("Productive  time",1,1);
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clr();
            return EVENT_HANDLED;
        }
        case TIME_TICK:{
            if (((protimer_tick_event_t *)(e))->ss == 10){
                mobj->active_state = IDLE;
                return EVENT_HANDLED;
            }
        }
    }
    return EVENT_IGNORED;
}

/************************Helper functions************************/
static void display_time(uint32_t time){
    char buf[7];
    String time_msg;
    uint16_t m = time/60;
    uint16_t s = time%60;
    sprintf(buf,"%03d:%02d",m,s);
    time_msg = (String)buf;
    lcd_set_cursor(5,0);
    lcd_print_string(time_msg);
}

static void display_msg(String s, uint8_t c, uint8_t r){
    lcd_set_cursor(r,c);
    lcd_print_string(s);
}

static void display_clr(void){
    lcd_clear();
}

static void do_beep(void){
    tone(PIN_BUZZER,4000,25);
}   