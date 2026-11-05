#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

/* Stubs before including main.c — Angles typedef lives only in main.c under HOST_TEST */
static bool mock_btn;
static bool last_manual_led;
static bool last_auto_led;

void set_manual_led(bool on) { last_manual_led = on; }
void set_auto_led(bool on) { last_auto_led = on; }
void set_roll(uint16_t pulse_us) { (void)pulse_us; }
void set_pitch(uint16_t pulse_us) { (void)pulse_us; }
void set_tilt(uint16_t pulse_us) { (void)pulse_us; }
bool GPIO_InputReadPin(void *port, uint8_t pin) {
    (void)port; (void)pin;
    return mock_btn;
}

#include "../assignment2/Src/main.c"

static void reset_mode(void) {
    curMode = MANUAL;
    pitch = FREE;
    roll = FREE;
    mock_btn = false;
    /* Drain edge state: one call with button low */
    fsm_mode();
    last_manual_led = true;
    last_auto_led = false;
    set_manual_led(true);
    set_auto_led(false);
}

int main(void) {
    /* 1) MANUAL + both CLAMPED → AUTO (no button) */
    reset_mode();
    pitch = CLAMPED;
    roll = CLAMPED;
    mock_btn = false;
    fsm_mode();
    assert(curMode == AUTO);
    assert(last_auto_led == true);
    assert(last_manual_led == false);

    /* 2) MANUAL + rising edge → AUTO */
    reset_mode();
    mock_btn = true;
    fsm_mode();
    assert(curMode == AUTO);

    /* 3) Held button does not re-toggle */
    pitch = FREE;
    roll = FREE;
    mock_btn = true;
    fsm_mode(); /* still held — must stay AUTO, not jump to MANUAL without a new edge after reset */
    assert(curMode == AUTO);

    /* 4) AUTO + rising edge + both FREE → MANUAL */
    reset_mode();
    curMode = AUTO;
    set_auto_led(true);
    set_manual_led(false);
    mock_btn = false;
    fsm_mode(); /* sync prev=low */
    pitch = FREE;
    roll = FREE;
    mock_btn = true;
    fsm_mode();
    assert(curMode == MANUAL);
    assert(last_manual_led == true);
    assert(last_auto_led == false);

    /* 5) AUTO + rising edge + not both FREE → stay AUTO */
    reset_mode();
    curMode = AUTO;
    set_auto_led(true);
    set_manual_led(false);
    mock_btn = false;
    fsm_mode();
    pitch = COMPLIANT;
    roll = FREE;
    mock_btn = true;
    fsm_mode();
    assert(curMode == AUTO);

    printf("test_mode_fsm: all passed\n");
    return 0;
}
