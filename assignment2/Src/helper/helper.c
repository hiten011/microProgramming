#include "helper.h"
#include <math.h>

#define RAD_TO_DEG 57.2957795f   // 180 / pi, converts radians to degrees

float calc_roll(float ay, float az) {
    return atan2f(ay, az) * RAD_TO_DEG;
}

float calc_pitch(float ax, float ay, float az) {
    return atan2f(-ax, sqrtf(ay * ay + az * az)) * RAD_TO_DEG;
}

float calc_tilt(float roll_deg, float pitch_deg) {
    return sqrtf(roll_deg * roll_deg + pitch_deg * pitch_deg);
}