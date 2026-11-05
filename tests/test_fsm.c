#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define HOST_TEST

static uint16_t last_pitch_us = 0;
static uint16_t last_roll_us = 0;
static uint16_t last_tilt_us = 0;

void set_pitch(uint16_t pulse_us) {
    last_pitch_us = pulse_us;
}

void set_roll(uint16_t pulse_us) {
    last_roll_us = pulse_us;
}

void set_tilt(uint16_t pulse_us) {
    last_tilt_us = pulse_us;
}

#include "../assignment2/Src/main.c"

void test_servo_deg_to_us(void) {
    printf("Testing servo_deg_to_us...\n");
    assert(servo_deg_to_us(0.0f) == 500);
    assert(servo_deg_to_us(90.0f) == 1500);
    assert(servo_deg_to_us(140.0f) == 2056);
    assert(servo_deg_to_us(40.0f) == 944);
    assert(servo_deg_to_us(180.0f) == 2500);
    assert(servo_deg_to_us(-10.0f) == 500);
    assert(servo_deg_to_us(200.0f) == 2500);
    printf("servo_deg_to_us tests passed!\n");
}

void test_pitch_fsm(void) {
    printf("Testing fsm_pitch...\n");

    // Initial state FREE
    pitch = FREE;
    angles.pitch = 0.0f;
    fsm_pitch();
    assert(pitch == FREE);
    assert(last_pitch_us == 1500);

    // Below 30 deg positive: remains FREE
    angles.pitch = 29.9f;
    fsm_pitch();
    assert(pitch == FREE);
    assert(last_pitch_us == 1500);

    // At or above 30 deg positive: transitions to COMPLIANT
    angles.pitch = 30.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(120.0f));

    // In COMPLIANT: deadband between 25.0 and 30.0 keeps COMPLIANT
    angles.pitch = 27.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(117.0f));

    // Below 25 deg: transitions back to FREE
    angles.pitch = 24.9f;
    fsm_pitch();
    assert(pitch == FREE);
    assert(last_pitch_us == 1500);

    // Negative angle: -30.0 deg transitions to COMPLIANT
    angles.pitch = -30.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(60.0f));

    // Negative angle: -35.0 deg in COMPLIANT
    angles.pitch = -35.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(55.0f));

    // Negative angle: -24.9 deg transitions back to FREE
    angles.pitch = -24.9f;
    fsm_pitch();
    assert(pitch == FREE);
    assert(last_pitch_us == 1500);

    // Transition COMPLIANT -> CLAMPED (> 50 deg)
    angles.pitch = 35.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(125.0f));

    angles.pitch = 50.0f; // boundary: > 50 required for clamped
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(140.0f));

    angles.pitch = 50.1f;
    fsm_pitch();
    assert(pitch == CLAMPED);
    assert(last_pitch_us == servo_deg_to_us(140.0f));

    angles.pitch = 55.0f;
    fsm_pitch();
    assert(pitch == CLAMPED);
    assert(last_pitch_us == servo_deg_to_us(140.0f));

    // In CLAMPED: deadband between 45.0 and 50.0 keeps CLAMPED
    angles.pitch = 48.0f;
    fsm_pitch();
    assert(pitch == CLAMPED);
    assert(last_pitch_us == servo_deg_to_us(140.0f));

    // At or below 45.0 deg: transitions back to COMPLIANT
    angles.pitch = 45.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(135.0f));

    // Negative angle clamping: < -50 deg
    angles.pitch = -50.1f;
    fsm_pitch();
    assert(pitch == CLAMPED);
    assert(last_pitch_us == servo_deg_to_us(40.0f));

    angles.pitch = -55.0f;
    fsm_pitch();
    assert(pitch == CLAMPED);
    assert(last_pitch_us == servo_deg_to_us(40.0f));

    // Negative angle release: >= -45.0 deg
    angles.pitch = -45.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);
    assert(last_pitch_us == servo_deg_to_us(45.0f));

    printf("fsm_pitch tests passed!\n");
}

void test_roll_fsm(void) {
    printf("Testing fsm_roll...\n");

    // Initial state FREE
    roll = FREE;
    angles.roll = 0.0f;
    fsm_roll();
    assert(roll == FREE);
    assert(last_roll_us == 1500);

    // Below 30 deg positive: remains FREE
    angles.roll = 29.9f;
    fsm_roll();
    assert(roll == FREE);
    assert(last_roll_us == 1500);

    // At or above 30 deg positive: transitions to COMPLIANT
    angles.roll = 30.0f;
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(120.0f));

    // In COMPLIANT: deadband between 25.0 and 30.0 keeps COMPLIANT
    angles.roll = 27.0f;
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(117.0f));

    // Below 25 deg: transitions back to FREE
    angles.roll = 24.9f;
    fsm_roll();
    assert(roll == FREE);
    assert(last_roll_us == 1500);

    // Negative angle: -30.0 deg transitions to COMPLIANT
    angles.roll = -30.0f;
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(60.0f));

    // Negative angle: -35.0 deg in COMPLIANT
    angles.roll = -35.0f;
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(55.0f));

    // Negative angle: -24.9 deg transitions back to FREE
    angles.roll = -24.9f;
    fsm_roll();
    assert(roll == FREE);
    assert(last_roll_us == 1500);

    // Transition COMPLIANT -> CLAMPED (> 50 deg)
    angles.roll = 35.0f;
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(125.0f));

    angles.roll = 50.0f; // boundary: > 50 required for clamped
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(140.0f));

    angles.roll = 50.1f;
    fsm_roll();
    assert(roll == CLAMPED);
    assert(last_roll_us == servo_deg_to_us(140.0f));

    angles.roll = 55.0f;
    fsm_roll();
    assert(roll == CLAMPED);
    assert(last_roll_us == servo_deg_to_us(140.0f));

    // In CLAMPED: deadband between 45.0 and 50.0 keeps CLAMPED
    angles.roll = 48.0f;
    fsm_roll();
    assert(roll == CLAMPED);
    assert(last_roll_us == servo_deg_to_us(140.0f));

    // At or below 45.0 deg: transitions back to COMPLIANT
    angles.roll = 45.0f;
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(135.0f));

    // Negative angle clamping: < -50 deg
    angles.roll = -50.1f;
    fsm_roll();
    assert(roll == CLAMPED);
    assert(last_roll_us == servo_deg_to_us(40.0f));

    angles.roll = -55.0f;
    fsm_roll();
    assert(roll == CLAMPED);
    assert(last_roll_us == servo_deg_to_us(40.0f));

    // Negative angle release: >= -45.0 deg
    angles.roll = -45.0f;
    fsm_roll();
    assert(roll == COMPLIANT);
    assert(last_roll_us == servo_deg_to_us(45.0f));

    printf("fsm_roll tests passed!\n");
}

void test_tilt_fsm(void) {
    printf("Testing fsm_tilt...\n");

    // Initial state FREE
    tilt = FREE;
    angles.tilt = 0.0f;
    fsm_tilt();
    assert(tilt == FREE);
    assert(last_tilt_us == 500);

    // At 20.0 deg: remains FREE (transition requires > 20 deg)
    angles.tilt = 20.0f;
    fsm_tilt();
    assert(tilt == FREE);
    assert(last_tilt_us == 500);

    // Above 20.0 deg: transitions to COMPLIANT
    angles.tilt = 20.1f;
    fsm_tilt();
    assert(tilt == COMPLIANT);
    assert(last_tilt_us == servo_deg_to_us(40.2f));

    // In COMPLIANT: deadband between 15.0 and 20.0 keeps COMPLIANT
    angles.tilt = 18.0f;
    fsm_tilt();
    assert(tilt == COMPLIANT);
    assert(last_tilt_us == servo_deg_to_us(36.0f));

    // Below 15 deg: transitions back to FREE
    angles.tilt = 14.9f;
    fsm_tilt();
    assert(tilt == FREE);
    assert(last_tilt_us == 500);

    // Transition COMPLIANT -> CLAMPED (>= 70 deg)
    angles.tilt = 25.0f;
    fsm_tilt();
    assert(tilt == COMPLIANT);
    assert(last_tilt_us == servo_deg_to_us(50.0f));

    angles.tilt = 50.0f;
    fsm_tilt();
    assert(tilt == COMPLIANT);
    assert(last_tilt_us == servo_deg_to_us(100.0f));

    angles.tilt = 69.9f;
    fsm_tilt();
    assert(tilt == COMPLIANT);
    assert(last_tilt_us == servo_deg_to_us(139.8f));

    angles.tilt = 70.0f; // boundary: >= 70 triggers clamped
    fsm_tilt();
    assert(tilt == CLAMPED);
    assert(last_tilt_us == servo_deg_to_us(140.0f));

    angles.tilt = 75.0f;
    fsm_tilt();
    assert(tilt == CLAMPED);
    assert(last_tilt_us == servo_deg_to_us(140.0f));

    // In CLAMPED: deadband between 65.0 and 70.0 keeps CLAMPED
    angles.tilt = 68.0f;
    fsm_tilt();
    assert(tilt == CLAMPED);
    assert(last_tilt_us == servo_deg_to_us(140.0f));

    // At or below 65.0 deg: transitions back to COMPLIANT
    angles.tilt = 65.0f;
    fsm_tilt();
    assert(tilt == COMPLIANT);
    assert(last_tilt_us == servo_deg_to_us(130.0f));

    printf("fsm_tilt tests passed!\n");
}

int main(void) {
    test_servo_deg_to_us();
    test_pitch_fsm();
    test_roll_fsm();
    test_tilt_fsm();
    printf("All current tests passed!\n");
    return 0;
}
