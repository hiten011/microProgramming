#ifndef HELPER_H
#define HELPER_H

#include <stdbool.h>
#include <stdint.h>

// MPU6050 raw accelerometer readings (signed 16-bit counts, 16384 LSB/g at ±2g)
typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
} MPU_Data;

// LEDs: true = on, false = off
void set_manual_led(bool on);
void set_auto_led(bool on);

// Servo pulse widths in microseconds
void set_roll(uint16_t pulse_us);
void set_pitch(uint16_t pulse_us);
void set_tilt(uint16_t pulse_us);

// Read raw accelerometer values from MPU6050 and store them in *data.
// Returns false if the I2C transfer fails (data is left unchanged on failure).
bool read_mpu_data(MPU_Data *data);

float calc_roll(float ay, float az);                    // phi   = atan2(ay, az)
float calc_pitch(float ax, float ay, float az);         // theta = atan2(-ax, sqrt(ay^2 + az^2))
float calc_tilt(float roll_deg, float pitch_deg);       // alpha = sqrt(theta^2 + phi^2)

#endif
