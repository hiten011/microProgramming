#ifndef HELPER_H
#define HELPER_H


float calc_roll(float ay, float az);                    // phi   = atan2(ay, az)
float calc_pitch(float ax, float ay, float az);         // theta = atan2(-ax, sqrt(ay^2 + az^2))
float calc_tilt(float roll_deg, float pitch_deg);       // alpha = sqrt(theta^2 + phi^2)

#endif