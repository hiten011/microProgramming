#include "helper.h"
#include "../stm_helper/my_stm_helper.h"
#include <math.h>

#define RAD_TO_DEG 57.2957795f   // 180 / pi, converts radians to degrees

void set_manual_led(bool on) {
    if (on) {
        GPIO_OutputSetPin(GPIOB, 11);    // manual LED on PB11
    } else {
        GPIO_OutputClearPin(GPIOB, 11);
    }
}

void set_auto_led(bool on) {
    if (on) {
        GPIO_OutputSetPin(GPIOA, 2);     // autopilot LED on PA2
    } else {
        GPIO_OutputClearPin(GPIOA, 2);
    }
}

// Helper to convert servo target angle (0 to 180 deg) to PWM pulse in microseconds (500 to 2500 us)
uint16_t servo_deg_to_us(float deg) {
    // Keep the command inside the servo's 0..180 deg travel.
    if (deg < 0.0f) {
        deg = 0.0f;
    } else if (deg > 180.0f) {
        deg = 180.0f;
    }
    return (uint16_t)roundf(500.0f + deg * (2000.0f / 180.0f));  // 500 us at 0 deg, 2500 us at 180
}

void set_roll(uint16_t pulse_us) {
    PWM_set_Pulse(TIM_CHANNEL_1, pulse_us);  // roll servo on TIM3 CH1 (PB4)
}

void set_pitch(uint16_t pulse_us) {
    PWM_set_Pulse(TIM_CHANNEL_2, pulse_us);  // pitch servo on TIM3 CH2 (PB5)
}

void set_tilt(uint16_t pulse_us) {
    PWM_set_Pulse(TIM_CHANNEL_3, pulse_us);  // tilt servo on TIM3 CH3 (PB0)
}

bool readAngles(Angles *out) {
    int16_t ax, ay, az;
    if (!MPU6050_ReadAccel(&ax, &ay, &az)) {
        return false;  // leave the previous angles if the sensor read fails
    }

    out->roll  = calc_roll((float)ay, (float)az);
    out->pitch = calc_pitch((float)ax, (float)ay, (float)az);
    out->tilt  = calc_tilt(out->roll, out->pitch);  // tilt is derived, not a raw axis
    return true;
}

float calc_roll(float ay, float az) {
    return atan2f(ay, az) * RAD_TO_DEG;  // phi, rotation about X
}

float calc_pitch(float ax, float ay, float az) {
    // Negate ax so nose-up reads as positive pitch.
    return atan2f(-ax, sqrtf(ay * ay + az * az)) * RAD_TO_DEG;
}

float calc_tilt(float roll_deg, float pitch_deg) {
    return sqrtf(roll_deg * roll_deg + pitch_deg * pitch_deg);  // combined tilt, sign dropped
}
