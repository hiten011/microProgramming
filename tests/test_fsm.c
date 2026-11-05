#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define HOST_TEST
#include "../assignment2/Src/main.c"

void test_pitch_fsm(void) {
    printf("Testing fsm_pitch...\n");

    // Initial state FREE
    pitch = FREE;
    angles.pitch = 0.0f;
    fsm_pitch();
    assert(pitch == FREE);

    // Below 30 deg positive: remains FREE
    angles.pitch = 29.9f;
    fsm_pitch();
    assert(pitch == FREE);

    // At or above 30 deg positive: transitions to COMPLIANT
    angles.pitch = 30.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);

    // In COMPLIANT: deadband between 25.0 and 30.0 keeps COMPLIANT
    angles.pitch = 27.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);

    // Below 25 deg: transitions back to FREE
    angles.pitch = 24.9f;
    fsm_pitch();
    assert(pitch == FREE);

    // Negative angle: -30.0 deg transitions to COMPLIANT
    angles.pitch = -30.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);

    // Negative angle: -24.9 deg transitions back to FREE
    angles.pitch = -24.9f;
    fsm_pitch();
    assert(pitch == FREE);

    // Transition COMPLIANT -> CLAMPED (> 50 deg)
    angles.pitch = 35.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);

    angles.pitch = 50.0f; // boundary: > 50 required for clamped
    fsm_pitch();
    assert(pitch == COMPLIANT);

    angles.pitch = 50.1f;
    fsm_pitch();
    assert(pitch == CLAMPED);

    // In CLAMPED: deadband between 45.0 and 50.0 keeps CLAMPED
    angles.pitch = 48.0f;
    fsm_pitch();
    assert(pitch == CLAMPED);

    // At or below 45.0 deg: transitions back to COMPLIANT
    angles.pitch = 45.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);

    // Negative angle clamping: < -50 deg
    angles.pitch = -50.1f;
    fsm_pitch();
    assert(pitch == CLAMPED);

    // Negative angle release: >= -45.0 deg
    angles.pitch = -45.0f;
    fsm_pitch();
    assert(pitch == COMPLIANT);

    printf("fsm_pitch tests passed!\n");
}

void test_roll_fsm(void) {
    printf("Testing fsm_roll...\n");

    // Initial state FREE
    roll = FREE;
    angles.roll = 0.0f;
    fsm_roll();
    assert(roll == FREE);

    // Below 30 deg positive: remains FREE
    angles.roll = 29.9f;
    fsm_roll();
    assert(roll == FREE);

    // At or above 30 deg positive: transitions to COMPLIANT
    angles.roll = 30.0f;
    fsm_roll();
    assert(roll == COMPLIANT);

    // In COMPLIANT: deadband between 25.0 and 30.0 keeps COMPLIANT
    angles.roll = 27.0f;
    fsm_roll();
    assert(roll == COMPLIANT);

    // Below 25 deg: transitions back to FREE
    angles.roll = 24.9f;
    fsm_roll();
    assert(roll == FREE);

    // Negative angle: -30.0 deg transitions to COMPLIANT
    angles.roll = -30.0f;
    fsm_roll();
    assert(roll == COMPLIANT);

    // Negative angle: -24.9 deg transitions back to FREE
    angles.roll = -24.9f;
    fsm_roll();
    assert(roll == FREE);

    // Transition COMPLIANT -> CLAMPED (> 50 deg)
    angles.roll = 35.0f;
    fsm_roll();
    assert(roll == COMPLIANT);

    angles.roll = 50.0f; // boundary: > 50 required for clamped
    fsm_roll();
    assert(roll == COMPLIANT);

    angles.roll = 50.1f;
    fsm_roll();
    assert(roll == CLAMPED);

    // In CLAMPED: deadband between 45.0 and 50.0 keeps CLAMPED
    angles.roll = 48.0f;
    fsm_roll();
    assert(roll == CLAMPED);

    // At or below 45.0 deg: transitions back to COMPLIANT
    angles.roll = 45.0f;
    fsm_roll();
    assert(roll == COMPLIANT);

    // Negative angle clamping: < -50 deg
    angles.roll = -50.1f;
    fsm_roll();
    assert(roll == CLAMPED);

    // Negative angle release: >= -45.0 deg
    angles.roll = -45.0f;
    fsm_roll();
    assert(roll == COMPLIANT);

    printf("fsm_roll tests passed!\n");
}

void test_tilt_fsm(void) {
    printf("Testing fsm_tilt...\n");

    // Initial state FREE
    tilt = FREE;
    angles.tilt = 0.0f;
    fsm_tilt();
    assert(tilt == FREE);

    // At 20.0 deg: remains FREE (transition requires > 20 deg)
    angles.tilt = 20.0f;
    fsm_tilt();
    assert(tilt == FREE);

    // Above 20.0 deg: transitions to COMPLIANT
    angles.tilt = 20.1f;
    fsm_tilt();
    assert(tilt == COMPLIANT);

    // In COMPLIANT: deadband between 15.0 and 20.0 keeps COMPLIANT
    angles.tilt = 18.0f;
    fsm_tilt();
    assert(tilt == COMPLIANT);

    // Below 15 deg: transitions back to FREE
    angles.tilt = 14.9f;
    fsm_tilt();
    assert(tilt == FREE);

    // Transition COMPLIANT -> CLAMPED (>= 70 deg)
    angles.tilt = 25.0f;
    fsm_tilt();
    assert(tilt == COMPLIANT);

    angles.tilt = 69.9f;
    fsm_tilt();
    assert(tilt == COMPLIANT);

    angles.tilt = 70.0f; // boundary: >= 70 triggers clamped
    fsm_tilt();
    assert(tilt == CLAMPED);

    // In CLAMPED: deadband between 65.0 and 70.0 keeps CLAMPED
    angles.tilt = 68.0f;
    fsm_tilt();
    assert(tilt == CLAMPED);

    // At or below 65.0 deg: transitions back to COMPLIANT
    angles.tilt = 65.0f;
    fsm_tilt();
    assert(tilt == COMPLIANT);

    printf("fsm_tilt tests passed!\n");
}

int main(void) {
    test_pitch_fsm();
    test_roll_fsm();
    test_tilt_fsm();
    printf("All current tests passed!\n");
    return 0;
}
