#ifndef HELPER_H
#define HELPER_H

#include <stdbool.h>
#include <stdint.h>

// Computed orientation angles from the MPU6050 accelerometer (all in degrees).
typedef struct {
    float roll;   // phi   = atan2(ay, az)
    float pitch;  // theta = atan2(-ax, sqrt(ay^2 + az^2))
    float tilt;   // alpha = sqrt(pitch^2 + roll^2)  — combined tilt magnitude
} Angles;

// LEDs: true = on, false = off
void set_manual_led(bool on);
void set_auto_led(bool on);

// Servo pulse widths in microseconds
void set_roll(uint16_t pulse_us);
void set_pitch(uint16_t pulse_us);
void set_tilt(uint16_t pulse_us);

bool readAngles(Angles *out);

float calc_roll(float ay, float az);                    // phi   = atan2(ay, az)
float calc_pitch(float ax, float ay, float az);         // theta = atan2(-ax, sqrt(ay^2 + az^2))
float calc_tilt(float roll_deg, float pitch_deg);       // alpha = sqrt(theta^2 + phi^2)

#endif
